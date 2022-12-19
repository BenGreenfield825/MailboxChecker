# MailboxChecker
Project for a device to monitor mail arriving in your mailbox

## Hardware
### Transmitter
There are two separate devices, a transmitter and a receiver. The transmitter is paired with a battery bank and is placed in the back of the mailbox. The device looks for a change in light to detect when the door has been opened and mail has been delivered.

![Transmitter](/misc/Transmitter.PNG)

### Receiver
The receiver sits somewhere in the house and waits for a radio signal from the transmitter.

![Receiver](/misc/Receiver.PNG)

### Components
Both devices use:
- Arduino Nano
- NRF24L01 Radio

The transmitter is paired with a "30,000mah" battery bank (cheap bank from Amazon, most likely to actually be around 20,000mah)

## Notes
- This project was designed to be used with a ***plastic*** mailbox. A metal mailbox would normally block the radio signal trying to be sent from inside the mailbox
    - To make this work with a metal mailbox, an extension might be able to be used and have the antenna on the outside of the box
- The transmitter goes through a "power cycle," where the device alternates between reading light values, going into sleep, and then sending a stayalive signal to keep the battery on. This is done to save on power without having to completely be put in sleep