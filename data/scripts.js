class servoSlider {
    constructor (
        servoID,
        outputID
    ) {
        this.name = servoID;
        this.servoSliderObj = document.getElementById(servoID);
        this.outputObj = document.getElementById(outputID);
        this.outputObj.innerHTML = this.servoSliderObj.value;
        this.val = Number(this.servoSliderObj.value);

        this.servoSliderObj.oninput = () => {
            this.val = Number(this.servoSliderObj.value);
            this.outputObj.innerHTML = this.val;
        }
    }
}

var baseHost;
var intervalID = null;

function sendInputs() {
    const query = `${baseHost}/servos?s1=${servo1.val}&s2=${servo2.val}&s3=${servo3.val}&s4=${servo4.val}`;
    console.log(query);
    fetch(query)
        .then(response => {
            console.log(`Inputs received! status: ${response.status}`);
        });
}

function startSendingInputs(interval) {
    if (intervalID) {
        clearInterval(intervalID);
    }

    intervalID = setInterval(() => {
        sendInputs();
    }, interval);

    console.log("Sending data at", interval, "ms.");
}

function stopSending() {
    if (intervalID) {
        clearInterval(intervalID);
        intervalID = null;
        console.log("Stopped sending data.");
    }
}

document.addEventListener('DOMContentLoaded', () => {
    baseHost = document.location.origin;
});




// Main program

var servo1 = new servoSlider("servo1", "out1");
var servo2 = new servoSlider("servo2", "out2");
var servo3 = new servoSlider("servo3", "out3");
var servo4 = new servoSlider("servo4", "out4");

startSendingInputs(100);
