"use strict";
import * as net from "net";


let client = new net.Socket;
let server_address = "127.0.0." + random_int(1, 254).toString();
let server_port = 2345;

console.log(`Attempting to connect to ${server_address}:${server_port}`);
client.connect(server_port, server_address, on_connect);


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

function encode_int(_input, _big_endian=false) {
  const result = new Uint8Array(4);
  let current = _input;
  for (let i = 0; i < 4; ++i) {
    result[_big_endian ? 3-i : i] = current & 0xFF;
    current >>= 8;
  }
  return result;
}

