# hackqueedo
Alternative solution to use torqueedo outboard 100x and 50x.

### situation
I got a trashed torqueedo t503 travel. Battery not responding to charging, throttle controller not responding to buttons press, motor no responding. 

After investigation:
- motor unit it self was ok?
- throttle unit flodet, salt water
- battery corroded cels ok? bms not responding.

### rebuild
- rebuild of battery. Removing bms, fixing bus bars, adding active balancer, atting hall sensor on off for output power, revareing
- throttle unit. Magnetic sensor is the same. It's in this case AS5045 12-bit programmable magnetic rotory position sensor. reused.
- motor unit. open. cleand. corosion on alluminium fixt with epoxy. closed

### components
- n-channel mosfet for on off output power supply
- active ballancer
- hall sensor switch I used AH277A from pc fan to ingage mosfet to have neutral pins without magnet in specific spot on the case of the battery. No drain off battery when on the shealf
- arduino mini
- oled 0.9
- dc/dc power supply to power arduino 5v from ~30v from main battery.
- rs485 to ttl module
- some led's some resistors

### overview v1.0
check it out :P https://youtu.be/9iYzMpFoCac


