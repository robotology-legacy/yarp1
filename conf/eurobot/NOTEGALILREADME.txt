NOTE ABOUT GALIL CARDS
By Carlos Beltran
---------------------

At the moment of writing, the eurobot setup has two galil cards; 
one for the arm Puma and another one for the EuroHead.
In the arm card case I have burned in the card EEPROM (BN command) some variables in order 
to guaranty security. Those parameters are:

KD - all to zero.
KI - all to zero.
KP - all to zero.
MO - all motors are in off state
FL5000 - This set the forward limit for the first joint to 5000. To avoid hitting the table
BL-3300 - This set the backward limit for the first joint to -3300. To avoid hitting the head

This could be considered paranoic, but by having these parameters burned into the card some extra 
security can be obtained. The motor control software should have to initialize these parameters to 
some adequate value in order to be able to move the arm. i.e. by using the SH (servo here) in order 
to activate the motor amplifiers. Also the PID values should be programmed with the correct values.
The limits have been calculated empirically (by moving the arm manually), and they are valid only 
for the first joint. The other joint have no limits. The initial position is marked with a red marker
in the arm base.
