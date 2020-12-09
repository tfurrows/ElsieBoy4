# ElsieBoy4
## Turn your Arduboy into an air-gapped LC4 appliance

There's a huge market of spies out there, who are tired of pushing around tiles just to encode and decode messages. But, they've been carefully trained to _never_ put their keys into a computer. What can they do?

Enter ElsieBoy4, an air-gapped solution with zero connectivity, and nothing stored between power cycles. Spies everywhere will gladly pitch their tile sets for this unique, pocket-sized hardware solution, capable of cute games or strong manual encryption.

Rather than provide an automatic means of encoding or decoding a message, ElsieBoy4 gives you just what you'd get from a tile set--the ability to manually step through the LC4 process. It is, in this sense, simply an appliance, but one that gives an added measure of speed and ease, when compared with human hands and little lettery chiclets.

## Usage

Using the B BUTTON, select one of the options described below:

1. *Key >* For viewing, entering, or modifying the LC4 key.
   You will be presented with an empty key to start. Use LEFT/RIGHT to move between key positions. Use UP/DOWN to select characters.
   Once a character is used, is becomes unavailable for selection (LC4 requires each character be used only once.)
   If you'd like, obtain something like a random key by mashing on the direction pad for a few minutes, then filling in the missing positions.
   As you navigate around the key, the selected character and associated LC4 indexes will show to the right.
   Press B to erase a character.
   Press A to return to the main menu.
2. *Nonce >* For viewing, entering, or modifying the LC4 nonce. 
   Navigation is similar to the Key entry.
   Only a completed six character nonce will be used, anything less will be ignored.
   A nonce is optional within this appliance.
   Press B to erase a character.
   Press A to return to the main menu.
3. *Encode or Decode >* For processing your text stream.
   A key is required. If you haven't entered a key, you will not be allowed to encode/decode.
   When you enter encode or decode mode, the key is copied to the key state, and the nonce is applied (if you entered one.)
   Each time you exit and return to encode/decode, you will have the option of resetting the key state and marker. This is useful if you need to restart your encode/decode session while keeping the key and nonce you entered.
   Within an encode/decode session, the UP button moves you forward in the alphabet, the DOWN button moves you backward. The RIGHT button moves you forward in the alphabet six characters at a time, the LEFT button moves you backward six.
   The result of your encoding/decoding is displayed in real time as you select your character.
   Press A to accept the character and complete the encode/decode by shifting the key state and moving the marker. You can then begin encoding the next character.
   Pess B to cancel the encode/decode and leave the key state and marker intact.
4. *Status >* For viewing key state and other stats.
   View the current key state and marker location, as well as the number of steps that have been completed.
   Press A or B to return to the main menu.


## Notes

The UI provides for an optional six character nonce. As I'm sure you're aware, you could simply include any size nonce with your message stream.

The UI does not provide for a signature, but you can append one to your message stream.


## License

This is free and unencumbered software released into the public domain. Do whatever you want with it. An UNLICENSE file has been included, which will self-destruct after reading.
