NOTE ABOUT GALIL CARDS
By Carlos Beltran
---------------------

In this moment the eurobot setup has two galil cards; one for the arm Puma and another one for the EuroHead.
In the arm card case I have burned in card EEPROM (BN command) some variables in order to guaranty security. Those parameters are:

KD - all to cero.
KI - all to cero.
KP - all to cero.
MO - all motors are in off state
FL5000 - This set the forward limit for the first joint to 5000. To avoid hitting the table
BL-3300 - This set the backward limit for the first joint to -3300- To avoid hitting the eurohead

This could be considered paranoic, but by having these parameters burn in the card some extra security can be obtain.
The motor control software should have to initialize these parameter to the adequate value in order to be able to move the arm. i.e. by using the SH (servo here) in order to activate the motor amplifiers. Also the PID values should be programmed with the correct values.
The limits have been calculated empirically (by moving the arm manually), and they are valid only for the first joint. The other joint have not limits. The initial position is marked with a red marker in the arm base.
