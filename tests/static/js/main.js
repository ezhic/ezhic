
var event_types = [];
event_types[0] = ['#2c7bb6','#abd9e9']; //SUCCESS
event_types[2] = ["#d7191c","#fdae61"]; //TIMEOUT
event_types[6] = ["magenta", "Violet"]; //CIRCUIT_BROKE
event_types[7] = ["magenta", "Violet"]; //CIRCUIT_FIXED
event_types[8] = ["#696969", "#2f4f4f"]; //NO_RUN

var g_maxY = 300; //max value for runtime
var STEP_WIDE = 32;
var timerHandle = -1;

var configs = {
        "nil" : {
            "kExceptions"            :            "2",
                    "kMend"            :            "3",
                    "kProbe"            :            "8",
                    "kTimeouts"            :            "3",
                    "mExceptions"            :            "40",
                    "mTimeouts"            :            "40",
                    "stickFor"            :            "1",
                    "timecount"            :            "1",
                    "timeout"            :            "1000",
                    "timestep"            :            "1"
        },

        "Uneventful" : {
            "kExceptions"            :            "2",
            "kMend"            :            "3",
            "kProbe"            :            "8",
            "kTimeouts"            :            "3",
            "mExceptions"            :            "40",
            "mTimeouts"            :            "40",
            "stickFor"            :            "1",
            "timecount"            :            "20",
            "timeout"            :            "1000",
            "timestep"            :            "15"
        },

        "Service failures" : {
            "kExceptions"            :            "20",
                    "kMend"            :            "30",
                    "kProbe"            :            "80",
                    "kTimeouts"            :            "30",
                    "mExceptions"            :            "40",
                    "mTimeouts"            :            "30",
                    "stickFor"            :            "12",
                    "timecount"            :            "20",
                    "timeout"            :            "195",
                    "timestep"            :            "15"
        },

        "With circuit breaker" : {
            "kExceptions"            :            "2",
                    "kMend"            :            "3",
                    "kProbe"            :            "6",
                    "kTimeouts"            :            "3",
                    "mExceptions"            :            "40",
                    "mTimeouts"            :            "4",
                    "stickFor"            :            "12",
                    "timecount"            :            "20",
                    "timeout"            :            "195",
                    "timestep"            :            "15"
        }
};
//draw 'point' number 'where' on a 'ctx' context
function drawPoint(point, ctx, where, isBroken) {
    var height = $("#canvas").height();
    var x = STEP_WIDE * where;
    var step = height/g_maxY;
    var h = point["duration"]*step;
    var width = STEP_WIDE - 8;
    if(point["duration"] < 0) {
        h = height;
        width = 8;
    }
    if(point["intention"] == 2) { //no run
        h = 3;
    }

    if(isBroken && point["type"] != 7) {
        ctx.fillStyle = "linen";
        ctx.fillRect(x, 0, STEP_WIDE, height);
    }

    var my_gradient=ctx.createLinearGradient(0,0,0,h);
    var colors = event_types[point["type"]];
    my_gradient.addColorStop(0,colors[0]);
    my_gradient.addColorStop(1,colors[1]);

    ctx.fillStyle = my_gradient;
    ctx.fillRect(x, height-h, width, h);
}

//get the last slice of data from the server (e.g., 32 points)
function doasyncget() {
    $.getJSON( "/", function( data ) {
        var canvas = $("#canvas");
        var ctx = canvas[0].getContext('2d');
        ctx.clearRect(0,0,canvas.width(),canvas.height());

        var broken = false;
        //var el = $('<div></div>');
        for(var i in data) {
            var point = data[i];
            if(point["type"] == 6) {
                broken = true;
            }
            //el.append($('<p>'+JSON.stringify(point)+'</p>'));
            drawPoint(point, ctx, i, broken);
            if(point["type"] == 7) {
                broken = false;
            }
        }
        //$( "#data" ).html(el);
    }).fail(function(msg) {
        console.log( "warning:");
        console.log(msg);
    });
}

//post the params to server (will trigger end-to-end restart)
function _injectParams(dataObj) {
    g_maxY = parseInt(dataObj["timecount"]) * parseInt(dataObj["timestep"]);
    console.log(dataObj);
    $.ajax({
                   type: "POST",
                   url: "/",
                   data: JSON.stringify(dataObj),
                   contentType: "application/json; charset=utf-8",
                   dataType: "json",
                   failure: function(errMsg) {
                console.log(errMsg);
            }
           });
}

function useConfig(config) {
    _injectParams(configs[config]);
    if(config != "nil") {
        $("#typetitle").text(config);
    }
}

function injectParams() {
    $("#param-form").hide();

    var dataObj = {};
    $("#param-form input").each(function(index, element) {
        dataObj[element.id] = element.value;
    });
    _injectParams(dataObj);
}

function toggleTimer() {
    if(timerHandle == -1) {
        timerHandle = setInterval(doasyncget,100);
    }
    else {
        clearInterval(timerHandle);
        timerHandle = -1;
    }
}

function init() {
    $('#param-form').hide();
    //toggleTimer();
    useConfig("nil");
}

function reveal() {
    Reveal.initialize({
        margin: 0,
        minScale: 1,
        maxScale: 1,
        width: 1000,
        height: 800,
        mouseWheel: true,
        center: false});
}