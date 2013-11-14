This is a simple tipptrainer designed to learn the neo keyboard layout.
Each session is five minutes long.
The tipptrainer detects bigram errors.
Each session the tipptrainer evaluates the user made errors and gives the user more words, where most errors occured.
(Implementationinfo: To each word is a errorvalue assigned, words are ordered in an array and sorted for errorvalue, a randomnumber (0;1) is powed 2 times multiplied with itself and the result multiplied with arraysize-1 to access a word with a high errorvalue but also other words)

