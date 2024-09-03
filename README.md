Devices connect via the socket package, with one as the server and the other as the client. The client sends a 4-bit message, encodes it into 7 bits, introduces 1-bit error, then sends it to the server. The server should successfully decode the 4 bits sent by the client.
1. Communication Within the Same Device
When you want to enable communication within the same device, you can set the IP address to 127.0.0.1. This IP address is known as "localhost" and is used for inter-process communication within the same device.
2. Communication Between Different Devices
When communicating between different devices, you need to set the IP address to the actual IP address of the target device. 
