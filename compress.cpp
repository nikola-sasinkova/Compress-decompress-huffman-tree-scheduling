#include "huffmanTree.h"

#include <fstream>
#include <bitset>
#include <string>
#include <iostream>

using namespace std;

// kolko bitov je potrebnych na zapis frekvencie f
int bitsForFreq(int f) {
    if (f <= 0) {
        return 0;
    }

    int bits = 0;
    while (f > 0) {
        bits++;
        f = f / 2; // delenie 2 - posun doprava
    }

    return bits;
}

void compress(string inputFile, string outputFile) {
    // vynulovanie frekvencnej mapy pre vsetky znaky
    for (int i = 0; i < 256; i++) {
        freqMap[(char)i] = 0;
    }

    // nacitanie celeho vstupneho suboru do premennej text
    ifstream in(inputFile.c_str(), ios::binary);

    string text;
    char charF;
    while (in.get(charF)) {
        text += charF;
    }
    in.close();

    // spocitam, kolkokrat sa v texte vyskytol kazdy znak
    for (size_t i = 0; i < (size_t)text.length(); i++) {
        char c = text[i];
        freqMap[c] = freqMap[c] + 1;
    }

    // vytvorenie Huffmanovho stromu z frekvencii
    Node* root = buildTree();

    // vygenerovanie Huff. kodov pre kazdy znak - codes
    codes.clear();
    generateCodes(root, "");

    // otvorenie vystupneho suboru v binarnom rezime
    ofstream out(outputFile.c_str(), ios::binary);

    // pocet bitov zakodovaneho textu
    uint64_t textBits = 0;

    // pocet bitov, kt. vytvaram pre header s frekvenciami
    uint64_t headerBits = 0;

    // pre kazdy znak spocitam kolko bitov zabere jeho kod - dlzka codes[ch] a kolko bitov potrebujem na zapis frekvencie - bitsForFreq
    for (int i = 0; i < 256; i++) {
        char ch = (char)i;
        int freq = freqMap[ch];
        int bits = bitsForFreq(freq);

        // celkovy pocet bitov na text
        textBits += freq * codes[ch].length();

        // celkovy pocet bitov na zapis frekvencii
        headerBits += (uint64_t)bits;
    }

    // zapisanie do suboru poctu bitov textu a poctu bitov headeru
    out.write((char*)&textBits, sizeof(uint64_t));
    out.write((char*)&headerBits, sizeof(uint64_t));

    // zapisanie frekvencii pre vsetky znaky do headeru - kolko bitov pouzijem na frekvenciu znaku - 1 bajt a samotnu frekvenciu v binarnom tvare
    for (int i = 0; i < 256; i++) {
        char ch = (char)i;
        int freq = freqMap[ch];
        int bits = bitsForFreq(freq);

        // kolko bitov pouzijem na zapis frekvencie
        unsigned char bitsCountByte = (unsigned char)bits;
        out.put((char)bitsCountByte);

        // ak je frekvencia 0 - pre tento znak dalej nic nezapisujem
        if (bits == 0) {
            continue;
        }

        // do premennej freqBits zapisem frekvenciu ako bity 0/1
        string freqBits = "";
        int temp = freq;

        // zapisujem bity zlava doprava
        for (int b = bits - 1; b >= 0; b--) {
            int mask = 1 << b;
            if ((temp & mask) != 0) {
                freqBits += '1';
            }
            else {
                freqBits += '0';
            }
        }

        // po 8 bitoch tieto bity prekonvertujem na bajt a zapisem
        while (freqBits.length() >= 8) {
            string first8 = freqBits.substr(0, 8);
            bitset<8> bs(first8);
            unsigned long value = bs.to_ulong();
            out.put((char)value);
            freqBits.erase(0, 8);
        }

        // ak ostane menej ako 8 bitov, doplnim ich nulami zprava a zapisem posledny bajt
        if (freqBits.length() > 0) {
            while (freqBits.length() < 8) {
                freqBits += '0';
            }

            bitset<8> bs(freqBits);
            unsigned long value = bs.to_ulong();
            out.put((char)value);
        }
    }

    // zakodovanie samotneho textu pomocou Huff. kodov
    string buffer = "";

    // pre kazdy znak v texte pripojim jeho Huff. kod do bufferu
    for (int i = 0; i < text.length(); i++) {
        char c = text[i];

        buffer += codes[c];
    }

    // buffer po 8 bitoch prekonvertujem na bajt a zapisem do suboru
    while (buffer.length() >= 8) {
        string first8_bits = buffer.substr(0, 8);
        bitset<8> bits(first8_bits);
        unsigned long value = bits.to_ulong();
        out.put((char)value);
        buffer.erase(0, 8);
    }

    // ak ostalo menej ako 8 bitov - doplnim nulami a zapisem posledny bajt
    if (buffer.length() > 0) {
        while (buffer.length() < 8) {
            buffer += '0';
        }

        bitset<8> bits(buffer);
        unsigned long value = bits.to_ulong();
        out.put((char)value);
    }

    out.close();
}

int main() {
    // cesty k vstupnemu a vystupnemu suboru
    string input = "C:\\Users\\nikola\\Documents\\file.txt";
    string output = "C:\\Users\\nikola\\Documents\\file_comp.txt";

    compress(input, output);

    return 0;
}