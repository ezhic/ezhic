###Motivation
Your perfect code frequently calls unreliable agents: 
databases, remote services, and highly inefficient algorithms.   
The calls can timeout, throw exceptions, return invalid results.   
The industry has invested much time and money in devising software patterns 
that attempt to minimise the impact of these routine errors.  
For a system to be tolerant to failures, we must recognize an ongoing problem 
and eliminate the calls entirely, thus relieving both your application and the already suffering agent.   
 
We introduce a mechanism for easily instrumenting the existing calls in your application.   
The added logic is recording the execution history for the different calls and provides tunable 
"circuit breakers" to cancel calls to misbehaving agents.  

###Usage
####Simple use: macros

Consider a code fetching data from remote server:
```C++
string value = service.get(key);
```

By wrapping it in `EZ` macros, we allow recording its execution history, and canceling the run if needed:
```C++
string value;
EZ_START("value from key")
value = service.get(key)
EZ_END
```

The macros create a scope, so we must define `value` before `EZ_START`, or it will not be available after EZ_END.  
The string `"value from key"` is the *label* of this execution.  
Executions with the same label also share history, and hence will be managed as one entity.  
The label is also bound to a particular configuration of the system.  
If no configuration was set for a given label, it will use the system defaults. 

####Advanced use: configuring bundles
Each label is associated with a single configuration manifest named *Bundle*.   
The default bundle is used for labels that were not assigned with any other bundle.   
Any configuration, including the default one, may be replaced at any point.   
However, this will not affect the bundles that were initialized as a copy of the default bundle.

Bundles encapsulate all of the configuration in `IntentionTagger` object and `AcceptUpdates` object.  
**Intention tagger** is responsible for circuit breaking logic &mdash; it is invoked before execution to decide  
 if it should take place and what sort of a run it is going to be: normal run, service probing, etc.   
**AcceptUpdate** is called after the execution and given the running time and completion status of the execution.  
   This component may be used for logging/auditing the runs, update the event history, and perform post-execution tasks. 
 
Usage example:  

```C++
Bundle bundle = make_mbundle(
                  std::cout,
                  timeout,
                  {kTimeouts, mTimeouts},
                  {kExceptions, mExceptions},
                  kProbe,
                  kMend);
Registry::instance().writeBundle(label, bundle);
```

Here we use a utility function to create a bundle that logs events to standard output,  
defines the duration of execution to be considered a timeout, and sets the different thresholds  
for circuit breaking and mending when timeouts or exceptions occur.  
Finally, we use `writeBundle` to associate the bundle with our label.   
All future invocations under label will use this registered bundle. 

####Using callables with BRunner
Often the call to unreliable service is already realized as a callable: free function, functor, a method of a class.  
In such case, it is more convenient to rely on `BRunner` utility class.  
We initialize the BRunner with a label and instrument the run be calling `run` and `run_m` methods for functions and methods respectively.

```C++
int call(string key); //free function; wraps a call to the service

struct A {
    float call(int a, int b); // method; wraps a call to the service 
};

struct F {
    void operator()(); //calls the service
}

Brunner br("calls to service");

//runs call with "key" as and argument, wraps the result in TNullable
TNullable<int> r1 = br.run(call, "key");
  

//runs A.call with 1 and 2 as arguments, wraps the result in TNullable
A a;
TNullable<float> r2 = br.run_m(a, A::call, 1, 2);

//runs F functor
TNullable<void> r3 = br.run(F());

```
####BRunner construction with user owned structures
When BRunner is constructed with a single label, it reaches for the global registry  
in order to obtain the bundle and event trace associated with the label.  
An alternative constructor allows to bypass the global registry completely and run using local structures:  

```C++
    Circuit circuit;
    Bundle bundle;
    LinuxSysTimer timer;
    BRunner br(circuit, bundle, &timer);
 ```
  

####Nesting
It is possible to nest runs (explicitly and implicitly), both made via EZ_ macros and BRunner.  
However, if inner and outer executions share a label the event trace may become quite confusing.  
To avoid such clashes your library code should use unique labels and possibly not rely on the global registry at all.   

### Components ###
* **Registry**: a central database for execution histories (EventTrace) and named configurations (Bundles).  
* **Bundle**: a configuration associated with a label.  
* **Task**: encapsulates single execution details: intention, start time, duration, status.   
* **Circuit predicates**: use an execution history to perform circuit operations: breaking, fixing, probing. 
* **Update acceptors**: receive a Task object, summarizing the execution for a label. 

####Registry
A registry allows setting and retrieving:  
a) A timer &mdash; a system timer or a dummy timer for adding timestamps to events and measuring running duration.  
b) Bundles &mdash; a bundle per label.  
c) Circuits &mdash; a circuit per label, contains the event history for that label.

####Bundle
Composed of: `IntentionTagger`, `AcceptUpdates`, and `isRethrow` flag.   
IntentionTagger is used prior to execution and applies circuit predicates to decide if the run should proceed.  
The decision may be *ROUTINE* execution, *PROBE* run, or *NO_RUN* at all.  
When the *isRethrow* flag is set to true, directs the runner to isRethrow any exceptions it catches.  
AcceptUpdates is given the status of the execution, even if the decision in the previous stage was *NO_RUN* or an exception was thrown.
   
####Circuit Predicates
We provide various simple circuit breakers triggering after observing K events of certain type in last M entries of a history.
* BreakAfterKTimeoutsInM  
* BreakAfterKExceptionsInM  
* BreakAfterKPercentsTimeoutsInM  
* BreakAfterKPercentsExceptionsInM  
  
Similarly, we introduce components for issuing probing calls and repairing a broken circuit:  
* ProbeAfterK  
* RepairAfterKSuccessInM  
* RepairAfterKPercentsSuccessInM  

More sophisticated predicates may be introduced by inheriting from `TracePredicateCloneable`.  
A single IntentionTagger will typically hold multiple circuit predicates.  
 
 ```C++
 IntentionTagger intentionTagger;

 intentionTagger.cloneIn(
     IntentionTagger::SHOULD_BREAK,
     Circuitry::BreakAfterKTimeoutsInM(2, 5));

 intentionTagger.cloneIn(
     IntentionTagger::SHOULD_BREAK,
     Circuitry::BreakAfterKExceptionsInM(2, 7));

 intentionTagger.cloneIn(
    IntentionTagger::SHOULD_PROBE,
    Circuitry::ProbeAfterK(4));

 intentionTagger.cloneIn(
     IntentionTagger::SHOULD_MEND,
     Circuitry::RepairAfterKSuccessInM(3));
 ```

####Acceptors
The acceptor is called after successful execution or catching an exception thrown inside the called code.  
The role of the acceptor is to analyze the execution metadata and return a single Event object to be stored in the event trace for the label.  
However, since multiple parties may be interested in that data, the one acceptor is often a facade to many different "subscribers".   
For instance, we may want to log the execution, check if timed out, validate or sanitize the result.   
We provide a utility class `AcceptorsArray` that will fanout the update to multiple acceptors and return the result of last invocation. 
 
####Post run call
Sometimes we want to apply specific logic to the result of the execution, rather than its metadata.  
To allow this, we expose a virtual `postRun` method which we call after the call to acceptors and just before returning the result.  
This mechanism is available only to runs executed via BRunner::run and BRunner::run_m.  
The virtual method has a single argument &mdash; reference to type-erased result object.  
In `Validator.h` we show two possible applications of this mechanism: validation and normalization of results.   
In both we inherit from BRunner and override postRun to execute our logic.  
`Validator` simply appends an Event::INVALID to event trace upon getting invalid result.  
`Recourse` fixes the result by assigning it with a sane value and appends Event::RECOURSE to the event trace.  
Result caching or normalization could be implemented in a similar way.   

    
####Shortcomings
* Overhead &mdash; the instrumentation of the call is not particularly costly,  
but it does involve a couple of lookups for strings in maps, copying objects by value, heap allocations, etc.  
We assume that the instrumented runs are not trivial tasks and usually take some time to complete,  
hence this overhead ought to be negligible. 

* Thread safety &mdash; not safe if the global registry is used by different threads.  
To avoid any race condition consider protecting non-const calls to registry with your favorite synchronization primitive.  

One possible solution to both is to avoid using the global registry, instead maintaining bundles and event traces local to threads. 