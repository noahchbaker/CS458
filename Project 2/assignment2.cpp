//CS458 Assignment #2: Written by Anna Vadella and Noah Baker
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <gmp.h>
#include <gmpxx.h>              //header for GMP package
#include <fstream>
#include <vector>
#include <algorithm>  // For std::reverse

using namespace std;

int numberGenerator(int digits) 
{
    int lowerBound = pow(10, digits - 1);
    int upperBound = pow(10, digits) - 1;
    return lowerBound + rand() % (upperBound - lowerBound);
}

void gcd(mpz_t result, const mpz_t a, const mpz_t b) 
{
    mpz_gcd(result, a, b);
}

void phiCalculation(mpz_t result, mpz_t p, mpz_t q) 
{
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(result, p, q);
    mpz_add_ui(p, p, 1);
    mpz_add_ui(q, q, 1); 
}

void inverseModulation(mpz_t result, const mpz_t a, const mpz_t m) 
{
    mpz_invert(result, a, m);
}

void modularExponentiation(mpz_t result, const mpz_t base, const mpz_t exponent, const mpz_t modulus) 
{
    mpz_powm(result, base, exponent, modulus);
}

//converts a string message to a Base36 integer
void messageToBase36(mpz_t result, const std::string &message) 
{
    mpz_init_set_ui(result, 0);
    for (char c : message) 
    {
        c = std::toupper(c);
        int value;
        if (c >= '0' && c <= '9') 
        {
            value = c - '0';
        } else if (c >= 'A' && c <= 'Z') 
        {
            value = c - 'A' + 10;
        } 
        else 
        {
            continue; // Ignore unsupported characters
        }
        mpz_mul_ui(result, result, 36);
        mpz_add_ui(result, result, value);
    }
}

//converts a Base36 integer back to a string message
std::string base36ToMessage(const mpz_t base36Value) 
{
    mpz_t value;
    mpz_init_set(value, base36Value);
    std::string result;
    while (mpz_cmp_ui(value, 0) > 0) 
    {
        mpz_t remainder;
        mpz_init(remainder);
        mpz_mod_ui(remainder, value, 36);
        int digit = mpz_get_ui(remainder);
        if (digit < 10)
            result += ('0' + digit);
        else
            result += ('A' + digit - 10);
        mpz_div_ui(value, value, 36);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

//calculates the block size
int calculateBlockSize(const mpz_t n) 
{
    size_t bitLength = mpz_sizeinbase(n, 36); 
    int blockSize = (bitLength / 8) - 1;
    return (blockSize < 1) ? 1 : blockSize;
}

//splits message by blocksize
std::vector<std::string> splitIntoBlocks(const std::string &input, int blockSize) 
{
    std::vector<std::string> blocks;
    int length = input.length();
    for (int i = 0; i < length; i += blockSize) {
        blocks.push_back(input.substr(i, blockSize));
    }
    return blocks;
}

int main() 
{
    int p_input, q_input, e_input, digits, value = 0;
    mpz_t p, q, n, d, phi_result, e, cipher, decrypt, converted_message, result;
    char keyType, fileChoice, privateChoice;
    string message, privateKeyFile, publicKeyFile, line, key;
    bool check;
    srand(time(0));

    mpz_inits(p, q, n, d, phi_result, e, cipher, decrypt, converted_message, result, nullptr);

    cout << "Do you wish to make/use a private key (p) or a public key (k)?: ";
    cin >> keyType;

    cout << "Will you be using a .txt file that you'd like to encrypt/decrypt? (Y or N): ";
    cin >> fileChoice;

    // Handling private key loading and decryption
    if((keyType == 'p' || keyType == 'P') && (fileChoice == 'y' || fileChoice == 'Y')) {
        cout << "Please enter the name of your file: ";
        cin >> privateKeyFile;

        ifstream privKeyFileRead(privateKeyFile);
        while(getline(privKeyFileRead, line)) {
            if(line[0] == '#') continue;  // Skipping comment lines
            stringstream ss(line);
            if(ss >> key >> value) {
                if (key == "n:")
                    mpz_set_ui(n, value);
                else if (key == "d:")
                    mpz_set_ui(d, value);
                else if (key == "p:")
                    mpz_set_ui(p, value);
                else if (key == "q:")
                    mpz_set_ui(q, value);
            }
        }
        privKeyFileRead.close();
        gmp_printf("Private Key:\n p: %Zd\n q: %Zd\n n: %Zd\n d: %Zd\n", p, q, n, d);

        // Message to decrypt
        cin.ignore(); //need this or it skips user input
        cout << "Enter the message you want to decrypt: ";
        getline(cin, message);

        std::string decryptedMessage;
        std::istringstream iss(message);
        std::string encryptedBlockStr;

        while (iss >> encryptedBlockStr) 
        {
            mpz_set_str(cipher, encryptedBlockStr.c_str(), 10); 
            modularExponentiation(decrypt, cipher, d, n); // Decryption step
            decryptedMessage += base36ToMessage(decrypt); //create decryption message as a string
        }

        // Display the decrypted message
        cout << "Decrypted message: " << decryptedMessage << endl;
    }
    // Handling public key loading and encryption
    else if((keyType == 'k' || keyType == 'K') && (fileChoice == 'y' || fileChoice == 'Y')) {
        cout << "Please enter the name of your file: ";
        cin >> publicKeyFile;

        ifstream pubKeyFileRead(publicKeyFile);
        while(getline(pubKeyFileRead, line)) {
            if(line[0] == '#') continue;  // Skipping comment lines
            stringstream ss(line);
            if(ss >> key >> value) {
                if (key == "n:")
                    mpz_set_ui(n, value);
                else if (key == "e:")
                    mpz_set_ui(e, value);
            }
        }
        pubKeyFileRead.close();

        gmp_printf("Public Key:\n n: %Zd\n e: %Zd\n", n, e);

        cin.ignore();  //without this it just skips user input
        cout << "Enter the message you want to encrypt: ";
        getline(cin, message);

        int blockSize = calculateBlockSize(n);

        // Split message into blocks
        std::vector<std::string> blocks = splitIntoBlocks(message, blockSize);
        string encryptedMessage;
        
        for (const std::string &block : blocks) 
        {
            messageToBase36(converted_message, block);
            modularExponentiation(cipher, converted_message, e, n); // encryption step
            encryptedMessage += std::string(mpz_get_str(NULL, 10, cipher)) + " "; //create ecnrypted string
        }

        // Remove the last space
        if (!encryptedMessage.empty()) 
        {
            encryptedMessage.pop_back();
        }

        cout << "Encrypted message: " << encryptedMessage << endl;
    }
    else if((keyType == 'p' || keyType == 'P') && (fileChoice == 'n' || fileChoice == 'N'))
    {
        cout << "Would you like to create a private key (c) or generate a public key from a private key (g)?: ";
        cin >> privateChoice;

        if(privateChoice == 'c' || privateChoice == 'C')            //creating private key
        {
            cout << "How many digits do you want your primes to be? ";
            cin >> digits;
            do 
            {
                mpz_set_ui(p, numberGenerator(digits));
                mpz_set_ui(q, numberGenerator(digits));
                if (mpz_probab_prime_p(p, 5) != 0 && mpz_probab_prime_p(q, 5) != 0 && mpz_cmp(p, q) != 0) 
                {
                    check = true;
                }
            } while (!check);

            // Compute n and phi(n)
            mpz_mul(n, p, q);
            gmp_printf("n: %Zd\n", n);

            phiCalculation(phi_result, p, q);

            // Choose e
            do 
            {
                cout << "Choose an e: ";
                cin >> e_input;
                mpz_set_ui(e, e_input);
                gcd(result, e, phi_result);
                if (mpz_cmp_si(result, 1) == 0) 
                {
                    cout << "Valid value for e!" << endl;
                    break;
                } 
                else 
                {
                    cout << "Invalid value for e" << endl;
                }
            } while (true);

            inverseModulation(d, e, phi_result);

            cout << "Enter the name of the file you want to store key in: ";
            cin >> privateKeyFile;

            ofstream privkeyFileWrite(privateKeyFile);
            privkeyFileWrite << "n: ";
            privkeyFileWrite << mpz_get_ui(n) << std::endl;
            privkeyFileWrite << "d: ";
            privkeyFileWrite << mpz_get_ui(d) << std::endl;
            privkeyFileWrite << "p: ";
            privkeyFileWrite << mpz_get_ui(p) << std::endl;
            privkeyFileWrite << "q: ";
            privkeyFileWrite << mpz_get_ui(q) << std::endl;

            privkeyFileWrite.close();
        }
        else if(privateChoice == 'g' || privateChoice == 'G')       //creating public key from private key
        {
            cout << "Please enter the name of your private key file: ";
            cin >> privateKeyFile;

            ifstream privKeyFileRead(privateKeyFile);
            while(getline(privKeyFileRead, line)) {
                if(line[0] == '#') continue;  // Skipping comment lines
                stringstream ss(line);
                if(ss >> key >> value) {
                    if (key == "n:")
                        mpz_set_ui(n, value);
                    else if (key == "d:")
                        mpz_set_ui(d, value);
                    else if (key == "p:")
                        mpz_set_ui(p, value);
                    else if (key == "q:")
                        mpz_set_ui(q, value);
                }
            }

            phiCalculation(phi_result, p, q);
            inverseModulation(e, d, phi_result);

            //file IO adding e and n to file
            cout << "Please enter the name of your public key file: ";
            cin >> publicKeyFile;

            ofstream pubKeyFileWrite(publicKeyFile);
            pubKeyFileWrite << "n: "; 
            pubKeyFileWrite << mpz_get_ui(n) << std::endl;
            pubKeyFileWrite << "e: ";
            pubKeyFileWrite << mpz_get_ui(e) << std::endl;

            pubKeyFileWrite.close();
        }
        else
        {
            cout << "ERROR: Invalid Selection." << endl;
        }
    }

    mpz_clears(p, q, n, d, phi_result, e, cipher, decrypt, converted_message, result, nullptr);
    return 0;
}