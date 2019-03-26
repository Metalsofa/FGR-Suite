#pragma once

#ifndef __crypt_h__
#define __crypt_h__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

/*The first character in this alphabet being the numeral '0' actually isn't great. You see, when
storing floats (in, say, graphical data) many of these floats will take on integral values. This
means (you guessed it) a LOT of zeroes appear in the decrypted text file. And in this type of ci
pher, the first character in the alphabet shows up as one of the characters in the cipher key. S
o if we use zeroes next to eachother a lot, the key begins to show up in the cryptogram, and then
they'll be onto us. Simple solution: move 0 to the end of the numerals in the cipher's base alpha
bet. Unfortunately this means that any graphics or cryptograms from before 1/22/19 at 10:45 PM wi
ll be rendered unreadable. If you really want to read some of those, use version of 'crypt.h' fro
m before then, or just move the 0 back to the beginning for a bit. I'm moving the 0 now. '1' will
be the new first numeral, which should not cause the same problem.*/
/*1/22/2019 10:56 PM: Actually, I gave it some more thought, and I see no reason that the alphabe
t should be ordered at all. It'll be WAY tougher to decrypt then! I bet there won't be a single
keyed vigenere cipher codebreaker program on the internet that could possible handle it then!*/
//Scrambling the alphabet now. Here's the old one: 
//"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz;:[]{}<>', .?/!@#$%^&*()-_=+|`~";
//Now for the new one, which includes the characters \ and " :
const string cipherOldAlphabet = //not for use
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz;:[]{}<>', .?/!@#$%^&*()-_=+|`~";
const string cipherBaseAlphabet = //very tough to decode
"+L~A0|J`F9Ir8?/O!t@E#u$NhiP7nQRS^(K)-_s1v[B]{M}<C>', .%wG&Hxyz;:=T*U6VWXY\\ZD5abcde4f\"gj3klmo2pq";

//Returns a shifted alphabet for the keyed vigenere cipher
string alphabetShift(char& newFirstletter, const string baseAlphabet);

int letterIndex(char& letter, const string& alphabet);

string encrypt(const string& message, const string& keyword);

string multiEncrypt(unsigned int depth, string message, string keyword);

string unencrypt(string cryptogram, string keyword);

string multiUnencrypt(unsigned int depth, string cipher, string key);

//Takes a filename and returns a vector where each line is an uncrypted line from the file.
vector<string> unencryptedContents(string filename, string keyword);

void encryptAndOverwrite(vector<string>& contents, string& filename, string keyword);

#endif