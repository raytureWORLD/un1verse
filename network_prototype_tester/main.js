"use strict";
import * as net from "net";


let client = new net.Socket;
let server_address = "127.0.0." + random_int(1, 254).toString();
let server_port = 2345;

console.log(`Attempting to connect to ${server_address}:${server_port}`);
client.connect(server_port, server_address, on_connect);
client.on("data", receive_message);


function on_connect() {
    console.log(`Connection established`);
    
    setInterval(send_message, 2000);
}

function send_message() {
    let message = random_string(5, 10);
    message += "\0";
    let packet_length = encode_int(4 + 4 + message.length);
    let packet_id = encode_int(1);

    console.log(`Sending: "${message}"`);

    client.write(packet_length);
    client.write(packet_id);
    client.write(message);
}

let header = new Array;
let read_message = "";
function receive_message(_data) {
    for(let element of _data) {
        if(header.length < 8) {
            header.push(element);
        } else {
            if(element != 0) {
                read_message += String.fromCharCode(element);
            } else {
                console.log(`Received: "${read_message}"`);

                header = new Array;
                read_message = "";
            }
        }
    }
}

/* inclusive */
function random_int(_min, _max) {
    return Math.floor(Math.random() * (_max - _min) + _min);
}

/* inclusive */
function random_string(_min, _max) {
    let result = "";
    let length = random_int(_min, _max);
    for(let i = 0; i < length; ++i) result += String.fromCharCode(random_int(97, 122));
    return result;
}

function encode_int(_input, _big_endian = false) {
    const result = new Uint8Array(4);
    let current = _input;
    for (let i = 0; i < 4; ++i) {
        result[_big_endian ? 3-i : i] = current & 0xFF;
        current >>= 8;
    }
    return result;
}

function decode_int(_input, _big_endian = false) {
    let result = 0;
    for (let i = 0; i < 4; ++i) {
        result += _input[_big_endian ? 3-i : i] << i;
    }
    return result;
}

