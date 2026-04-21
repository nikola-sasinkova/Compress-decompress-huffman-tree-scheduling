#include "huffmanTree.h"

#include <fstream>
#include <bitset>
#include <string>
#include <iostream>
#include <cstdint>

using namespace std;

void decompress(string inputFile, string outputFile) {

    // otvorenie vstupneho suboru v binarnom rezime
    ifstream in(inputFile.c_str(), ios::binary);

    // precitanie poctu bitov zakodovaneho textu
    uint64_t textBits = 0;
    in.read((char*)&textBits, sizeof(uint64_t));

    // precitanie poctu bitov v headeri - frekvencie
    uint64_t headerBits = 0;
    in.read((char*)&headerBits, sizeof(uint64_t));

    // vynulovanie frekvencnej mapy
    for (int i = 0; i < 256; i++) {
        freqMap[(char)i] = 0;
    }

    // nacitanie frekvencii pre vsetky znaky
    for (int i = 0; i < 256; i++) {
        unsigned char bitsCountByte = 0;

        // precitam 1 bajt a zistim pocet bitov na frekvenciu znaku
        in.read((char*)&bitsCountByte, 1);

        int bitsCount = (int)bitsCountByte;

        // ak frekvencia znaku je 0 - nic necitam
        if (bitsCount == 0) {
            freqMap[(char)i] = 0;
            continue;
        }

        // pocet bajtov pouzitych na zapis tychto bitov frekvencie
        int codeBytes = (bitsCount + 7) / 8;

        string freqBits = "";
        // nacitanie prislusneho poctu bajtov a skonvertovanie na 0/1
        for (int b = 0; b < codeBytes; b++) {
            unsigned char byte = 0;
            in.read((char*)&byte, 1);

            bitset<8> bs(byte);
            freqBits += bs.to_string();
        }

        // ziskanie frekvencie z prvych bitsCount bitov
        int freq = 0;
        for (int k = 0; k < bitsCount; k++) {
            freq <<= 1; // posun vlavo
            if (freqBits[k] == '1') {
                freq |= 1; // nastavenie bitu, ktory je najviac vpravo na 1
            }
        }

        // ulozenie frekvencie pre znak i
        freqMap[(char)i] = freq;
    }

    // na zaklade frekvencii - rekonstrukcia Huff. stromu
    Node* root = buildTree();

    // nacitanie zvysku suboru - postupnost bitov
    string bits = "";
    unsigned char byte = 0;
    while (in.read((char*)&byte, 1)) {
        bitset<8> b(byte);
        bits += b.to_string();
    }
    in.close();

    // otvorenie vystupneho suboru
    ofstream out(outputFile.c_str(), ios::binary);

    Node* current = root;
    int decoded = 0; // pocet dekodovanych znakov

    // nemozem pouzit viac bitov ako som zapisala do textBits
    uint64_t maxBitsAvailable = (uint64_t)bits.size();
    uint64_t limit = (textBits < maxBitsAvailable) ? textBits : maxBitsAvailable;

    // prechadzanie bitu po bite v Huff. strome
    for (uint64_t i = 0; i < limit; i++) {
        char bit = bits[(size_t)i];

        // podla bitu idem dolava = 0 alebo doprava = 1
        if (bit == '0') {
            current = current->left;
        }
        else {
            current = current->right;
        }

        // ak som na liste - znak, zapisem ho do vystupneho suboru
        if (current != nullptr && current->left == nullptr && current->right == nullptr) {
            
            out.put(current->ch); // dekodovany znak
            decoded++;
            current = root; // vratim sa na koren, aby som dekodovala dalsi znak
        }
    }

    out.close();
}

int main() {
    // cesty k vstupnemu a vystupnemu suboru
    string input = "C:\\Users\\nikola\\Documents\\file_comp.txt";
    string output = "C:\\Users\\nikola\\Documents\\file_uncomp.txt";

    decompress(input, output);

    return 0;
}