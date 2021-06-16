"use strict";
import * as net from "net";

let client = new net.Socket;
let server_address = "127.0.0." + random_inclusive(1, 254).toString();
let server_port = 2345;

console.log(`Attempting to connect to ${server_address}:${server_port}`);
client.connect(server_port, server_address, on_connect);



function random_inclusive(_min, _max) {
    return Math.floor(Math.random() * _min) + _max;
}

function on_connect() {
    console.log(`Connection established`);
}