/* The following are the message types which can be sent
   across the network, along with explanations of what
   data is sent along with the message. This data is
   encoded and decoded in the WritePacket and ReadPacket
   classes.
 */

/* CLIENT TO SERVER

   * CONNECT - no additional info.

   * DISCONNECT - no additional info.

   * USER_STATE - contains angle (float) and keypress
        state (char).

 */

#define CTS_CONNECT 0
#define CTS_DISCONNECT 1
#define CTS_USER_STATE 2

/* SERVER TO CLIENT

   * INITIAL_ANGLE - one float

   * WORLD_DATA - all the information about the world.
        See World::sendObjects and World::receiveObjects
        for the data.

 */

#define STC_INITIAL_ANGLE 0
#define STC_WORLD_DATA 1
