//CS458 Assignment #1: Written by Noah Baker and Anna Vadella
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

//function declarations
char shift(char, int, bool);
string substitution(string, string);
string affine(string, int, int, bool);
string vignere(string, string, bool);

// function for shift cipher
char shift(char x, int k, bool encrypt)
{
    // True means encypt, false means decrypt
    int y = 0;
    if(encrypt == true)                 //encrypt message
    {
        if(x >= 'A' && x <= 'Z')        //uppercase
        {
            y = x + k;
            if (y > 'Z')
                y = y - 'Z' + 'A' - 1;
            return (char)y;
        }
        if(x >= 'a' && x <= 'z')           //lowercase
        {
            y = x + k;
            if (y > 'z')
                y = y - 'z' + 'a' - 1;
            return (char)y;
        }

        // otherwise
        return x;
    }
    else                                 //decrypt message
    {
        if(x >= 'A' && x <= 'Z')         //uppecase
        {
            y = x - k;
            if(y < 'A')
                y = y + 'Z' - 'A' + 1;
            return (char)y;
        }
        if(x >= 'a' && x <= 'z')         //lowercase
        {
            y = x - k;
            if(y < 'a')
                y = y + 'z' - 'a' + 1;
            return (char)y;
        }

        // otherwise
        return x;
    }
}

// function for substitution cipher
string substitution(string keyword, string message, bool encrypt)
{
    string cipher;
    string result;
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    bool used[26] = { false };  // To track which letters are used in the cipher

    for (int i = 0; i < keyword.length(); i++) {
        if (!used[keyword[i] - 'a']) {  // Only add unique letters from the keyword
            cipher += keyword[i];
            used[keyword[i] - 'a'] = true;  // Mark this letter as used
        }
        else
        {
            // do nothing
        }
    }

    int alphabetIndex = ((keyword.length() - 1) - 'a' + 1) % 26;  // Start from the letter after the last keyword letter

    // Add remaining letters in the alphabet, wrapping around if necessary
    for (int i = 0; i < 26; i++) {
        char currentLetter = alphabet[alphabetIndex];
        if (!used[alphabetIndex]) {
            cipher += currentLetter;
            used[alphabetIndex] = true;
        }
        alphabetIndex = (alphabetIndex + 1) % 26; 
    }

    cout << "Cipher: " << cipher << endl;  // Output the generated cipher 

    int position = 0;

    if(encrypt == true)
    {
        for(int i = 0; i < message.length(); i++)
        {
            if(isupper(message[i]))
            {
                position = message[i] - 'A';
                result += toupper(cipher[position]);
            }
            else if(islower(message[i]))
            {
                position = message[i] - 'a';
                result += cipher[position];
            }
            else
            {
                result+= message[i];
            }
        }

        cout << result << endl;
        return result;
    }
    else
    {
        for(int i = 0; i < message.length(); i++)
        {
            if(isupper(message[i]))
            {
                char encrypted_char = tolower(message[i]);
                position = cipher.find(encrypted_char);
                result += toupper(alphabet[position]);
            }
            else if(islower(message[i]))
            {
                position = cipher.find(message[i]);
                result += alphabet[position];
            }
            else
            {
                result+= message[i];
            }
        }

        cout << result << endl;
        return result;
    }
}

// function for affine cipher
string affine(string message, int a, int b, bool encrypt)
{
    string returnMessage;
    if(encrypt == true)
    {
        for(int i = 0; i < message.length(); i++)
        {
            //uppercase
            if(message[i] >= 'A' && message[i] <= 'Z')
            {
                returnMessage += (char) ((((a * (message[i]-'A') ) + b) % 26) + 'A');
            }
            else if(message[i] >= 'a' && message[i] <= 'z')
            {
                returnMessage += (char) ((((a * (message[i]-'a') ) + b) % 26) + 'a');
            }
            else
                returnMessage += message[i];
        }
        return returnMessage;
    }
    else
    {
        int inverse = 0;
        for(int i = 0; i < 26; i++)
        {
            // a inverse is when a times inverse == 1
            if( (a * i) % 26 == 1)
            {
                inverse = i;
            }
        }

        for(int i = 0; i < message.length(); i++)
        {

            //uppercase
            if(message[i] >= 'A' && message[i] <= 'Z')
            {
                returnMessage += (char)((inverse * ((message[i] - 'A' - b + 26)) % 26) + 'A');
            }
            else if(message[i] >= 'a' && message[i] <= 'z')
            {
                returnMessage += (char)((inverse * ((message[i] - 'a' - b + 26)) % 26) + 'a');
            }
            else
                returnMessage += message[i];
        }
        return returnMessage;
    }
}

// function for vigenere cipher
string vigenere(string message, string keyword, bool encrypt)
{
    string result;
    char position; 

    for (int i = 0, j = 0; i < message.length(); i++) 
    {
        char currentChar = message[i];
        if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) //works for upper and lower
        { 
            char cipherChar = keyword[j % keyword.length()];
            int keyShift;

            if (cipherChar >= 'A' && cipherChar <= 'Z')         //uppercase
                keyShift = cipherChar - 'A'; 
            else                                                //lowercase
                keyShift = cipherChar - 'a';

            j++;

            if (isupper(currentChar))               //char used to calculate position of letters in alphabet
                position = 'A';
            else 
                position = 'a';

            if (encrypt) 
                result += (currentChar + keyShift - position) % 26 + position;
            else 
                result += (currentChar - keyShift + 26 - position) % 26 + position;
        } 
        else                          
            result += currentChar;
    }
    return result;
}

int main()
{
    //variable declarations
	ifstream fileRead;
    ofstream fileWrite;
	string inputFile;
    string outputFile;

    // This currently works if everything is on one line, thats how sorenson did it in his example, but we can ask and
    // maybe do vectors or something so we can do it across multiple lines
    string message;
    string line;

    //oops string works good for affine lol
    string result;

    int FirstOption = 0; 
    char SecondOption = ' ';

	// ask user for input text file name
	cout << "Enter the name of your input file (.txt): ";
	cin >> inputFile;

    fileRead.open(inputFile);
    if (!fileRead.is_open()) 
    {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }
    else 
    {
        while (!fileRead.eof()) 
        {
            getline(fileRead, line);
            message = message + line + " ";
        }
    }
    fileRead.close();

    // ask the user for the output text file name
    cout << "Enter the name of your output file (.txt): ";
	cin >> outputFile;

	// ask user for key (or name of a file containing the key)
	int key = 0;
    string keyword;

	// encryption algorithm menu -- probs turn this into a function
	cout << "1) Shift Cipher" << endl;
	cout << "2) Substitution Cipher" << endl;
	cout << "3) Affine Cipher" << endl;
	cout << "4) Vigenere Cipher" << endl;
    cout << "5) Quit " << endl;

	cout << "Please enter your choice: ";
	cin >> FirstOption;

    // ask user to encrypt or decrypt -- changed this to a function
	cout << "Would you like to encrypt (E) or decrypt (D)?: ";
    cin >> SecondOption;

    if(FirstOption == 1)                    //Shift Cipher
    {
        cout << "Enter your key (0 - 25): ";
        cin >> key;

        if(SecondOption == 'E' || SecondOption == 'e')
        {
            for(int i = 0; i < message.length(); i++)
                //true means encypt
                message[i] = shift(message[i],key, true);
                cout << message << endl;
        }
        else if(SecondOption == 'D' || SecondOption == 'd')
        {
            for(int i = 0; i < message.length(); i++)
                //false means decrypt
                message[i] = shift(message[i],key, false);
                cout << message << endl;
        }
        else
            cout << "That is not a valid selection, please try again." << endl;

        // Writes the result to the output file specified by the user!
        fileWrite.open(outputFile);
        fileWrite << message << endl;
        fileWrite.close();
    }
    else if (FirstOption == 2)                  //Substitution Cipher
    {
        cout << "Enter the keyword: ";
        cin >> keyword;

        if(SecondOption == 'E' || SecondOption == 'e')
        {
            result = substitution(keyword, message, true);
        }
        else if(SecondOption == 'D' || SecondOption == 'd')
        {
            result = substitution(keyword, message, false);
        }
        else
            cout << "That is not a valid selection, please try again." << endl;

        fileWrite.open(outputFile);
        fileWrite << result << endl;
        fileWrite.close();

    }
    else if (FirstOption == 3)                  //Affine Cipher        
    {
        int a,b = 0;
        cout << "Enter the value of a: ";
        cin >> a;

        if(a % 2 == 0 || a % 13 == 0)
        {
            cout << "Bad value for a" << endl;
        }

        cout << "Enter the value of b: ";
        cin >> b;

        if(SecondOption == 'E' || SecondOption == 'e')
        {
            result = affine(message, a, b, true);
            cout << result << endl;
        }
        else if(SecondOption == 'D' || SecondOption == 'd')
        {
            result = affine(message, a, b, false);
            cout << result << endl;
        }
        else
            cout << "That is not a valid selection, please try again." << endl;

        fileWrite.open(outputFile);
        fileWrite << result << endl;
        fileWrite.close();
    }
    else if(FirstOption == 4)                   //Vigenere Cipher
    {
        cout << "Enter the keyword: ";
        cin >> keyword;

        if(SecondOption == 'E' || SecondOption == 'e')
        {
            result = vigenere(message, keyword, true);
            cout << result << endl;   
        }
        else if(SecondOption == 'D' || SecondOption == 'd')
        {
            result = vigenere(message, keyword, false);
            cout << result << endl;   
        }
        else
            cout << "That is not a valid selection, please try again." << endl;

        fileWrite.open(outputFile);
        fileWrite << result << endl;
        fileWrite.close();
    }
    else
        cout << "That is not a valid selection, please try again." << endl;
    
    return 0;
}