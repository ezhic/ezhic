# Ёzhic C++ Library ![ЁzHiC](docs/ezhic.png)  #

*ЁzHiC* stands for **E**a**z**y **Hi**gh **C**onstancy*.   
*Constancy: the quality of being faithful and dependable.

*ЁzHiC* is inspired by Netflix [Hystrix](https://github.com/Netflix/Hystrix) (for Java) and [Pycopine](https://github.com/wiliamsouza/hystrix-py) (for Python).  
However, it is not a *C++* clone of the above, but an independent API, implemented from first principles.   

*ЁzHiC* is used to instrument long running tasks, in particular, unreliable remote service requests.  
Thus, we centralize the logic for automating the following tasks:
* execution audit: running time, result, exceptions.
* circuit-breaking for cancelling hopeless tasks; and further mending of the circuit.
* request batching and result caching
* result validation and normalization

The library is C++03 friendly. 


[Documentation](docs/index.md)