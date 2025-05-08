class ServoSlider {
    constructor (
        servoID,
        outputID
    ) {
        this.name = servoID;
        this.ServoSliderObj = document.getElementById(servoID);
        this.outputObj = document.getElementById(outputID);
        this.outputObj.innerHTML = this.ServoSliderObj.value;
        this.val = Number(this.ServoSliderObj.value);

        this.ServoSliderObj.oninput = () => {
            this.val = Number(this.ServoSliderObj.value);
            this.outputObj.innerHTML = this.val;
        }
    }
}

class LineState {
    constructor(lineID) {
        this.name = lineID;
        this.LineStateObj = document.getElementById(lineID);
        this.LineStateObj.style.background = "red";
    }

    update(state) {
        if (state == true) this.LineStateObj.style.background = "green";
        else this.LineStateObj.style.background = "red";
    }
}

var baseHost;
var intervalID = null;
var updatesBuffer = "linestate:0;distance1:-1;distance2:-1;distance3:-1;KP:4.0;KI:0.0;KD:0.0;left:0;right:0;distance4:-1;mode:0";
var updates = Object.fromEntries(
    updatesBuffer.split(';')
        .filter(entry => entry)
        .map(pair => {
            const [key, value] = pair.split(':');
            return [key, Number(value)];
        })
);

function sendInputs() {
    const servo1_val = servo1.val;
    const servo2_val = servo2.val;
    const servo3_val = 180 - servo2.val;
    const servo4_val = 180 - servo1.val;
    const query = `${baseHost}/servos?s1=${servo1_val}&s2=${servo2_val}&s3=${servo3_val}&s4=${servo4_val}&kp=${kp.val}&ki=${ki.val}&kd=${kd.val}`;
    console.log(query);
    fetch(query)
        .then(response => {
            console.log(`Inputs received! status: ${response.status}`);
        });
}

function getUpdates() {
    const query = `${baseHost}/updates`;
    fetch(query)
        .then(response => response.text())
        .then(data => {
            updatesBuffer = data;
        })
    console.log(updates);
}

function startSendingQueries(interval) {
    if (intervalID) {
        clearInterval(intervalID);
    }

    intervalID = setInterval(() => {
        sendInputs();
        getUpdates();
        reportValues();
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

function isLineOn(val, pos) {
    const binary = Number(val);
    const test = ((binary >> pos) & 1);
    if (test == 1) return true;
    else return false;
}

function reportValues() {
    updates = Object.fromEntries(
        updatesBuffer.split(';')
            .filter(entry => entry)
            .map(pair => {
                const [key, value] = pair.split(':');
                return [key, Number(value)];
            })
    );

    // Linestate
    const l1 = isLineOn(updates.linestate, 3);
    const l2 = isLineOn(updates.linestate, 2);
    const l3 = isLineOn(updates.linestate, 1);
    const l4 = isLineOn(updates.linestate, 0);

    line1.update(l1);
    line2.update(l2);
    line3.update(l3);
    line4.update(l4);

    var bot_mode = "";

    if (updates.mode === 0) {
        bot_mode = "LF";
    } else if (updates.mode === 1) { 
        bot_mode = "TC";
    } else if (updates.mode === 2) {
        bot_mode = "OA";
    }

    // Distances
    const distance_buffer = `Distance 1: ${updates.distance1} || Distance 2: ${updates.distance2} || Distance 3: ${updates.distance3} || Distance 4: ${updates.distance4} || Mode: ${bot_mode}`;
    distances_out.innerHTML = distance_buffer;

    // Motors
    const motors_buffer = `Left: ${updates.left} || Right: ${updates.right}`;
    motors_out.innerHTML = motors_buffer;
}

document.addEventListener('DOMContentLoaded', () => {
    baseHost = document.location.origin;
});




// Main program

var servo1 = new ServoSlider("servo1", "out1");
var servo2 = new ServoSlider("servo2", "out2");
var kp = new ServoSlider("kp", "kp_out");
var ki = new ServoSlider("ki", "ki_out");
var kd = new ServoSlider("kd", "kd_out");
var line1 = new LineState("line1");
var line2 = new LineState("line2");
var line3 = new LineState("line3");
var line4 = new LineState("line4");
var distances_out = document.getElementById("distances_out");
var motors_out = document.getElementById("motors_out");

startSendingQueries(100);
