
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <math.h>

using namespace std;

string trimStr(string in)
{
	string out = in;
	bool reachedFirstChar = false;
	bool reachedLastChar = false;
	int startSpacesAmout = 0;
	int endSpacesAmout = 0;

	for (size_t i = 0; i < in.length(); i++)
	{
		if (!reachedFirstChar) reachedFirstChar = !isspace(out[i]);
		if (reachedFirstChar) continue;
		if (isspace(out[i])) startSpacesAmout++;
	}

	out = out.substr(startSpacesAmout, out.length() - startSpacesAmout);

	for (size_t i = 0; i < out.length(); i++)
	{
		if (!reachedLastChar) reachedLastChar = !isspace(out[out.length() - i - 1]);
		if (reachedLastChar) continue;
		if (isspace(out[out.length() - i - 1])) endSpacesAmout++;
	}

	out = out.substr(0, out.length() - endSpacesAmout);

	return out;
}

// hashes int with MurmurHash3 hash func
uint32_t hashCode(int32_t num, size_t len, uint32_t seed = 0) {
	std::string str = std::string(reinterpret_cast<char*>(&num), sizeof(num));

	uint32_t h1 = 0xdeadbeef ^ seed;
	uint32_t h2 = 0x2bc2bc2b ^ seed;
	const uint32_t c1 = 0x85ebca6b;
	const uint32_t c2 = 0xc2b2ae35;

	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.data());
	const int nblocks = len / 4;

	for (int i = 0; i < nblocks; i++) {
		uint32_t k1 = reinterpret_cast<const uint32_t*>(data)[i];
		k1 *= c1;
		k1 = (k1 << 15) | (k1 >> 17);
		k1 *= c2;

		h1 ^= k1;
		h1 = (h1 << 13) | (h1 >> 19);
		h1 = h1 * 5 + 0xe6546b64;
	}

	const uint8_t* tail = data + nblocks * 4;
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];
		k1 *= c1;
		k1 = (k1 << 15) | (k1 >> 17);
		k1 *= c2;
		h1 ^= k1;
	}

	h1 ^= len;
	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;

	return h1;
}

string toLowercase(string str)
{
	string out = str;
	for (size_t i = 0; i < out.length(); i++)
	{
		out[i] = tolower(out[i]);
	}

	return out;
}

float randomNumber()
{	
	srand(hashCode(time(0), 64));
	float rnd = ((float)rand()) / 1000;
	return rnd - floor(rnd);
}

float parseToNumber(string str)
{

	if (str == "rnd")
	{
		return randomNumber();
	}
	
	if (str == "pi")
	{
		return atan(1) * 4;
	}

	return stof(str);
}

int main()
{
	float lastResult;
    bool lastResultExists = false;
	string validOperators[4] = { "+", "-", "*", "/" };
	bool commentMode = false;

    cout <<      "  -- WARNING -- Your terminal doesn't support replacing previous lines with \\033[F, you may see \"F\" sometimes lol." << endl;
    cout << "\033[F\r                                                                                                                  " << endl;
    // warning for terminals that dont support moving the cursor up a line. if term supports it directly replaces the warning afterwards.


    cout << "  C++ CALCULATOR ft. comments lol" << endl;
    cout << "  Enter a numeric expression below: " << endl;
    cout << endl;

	while (true)
	{
		if (commentMode)
		{
			cout << "\r/ ";
		}
		else
		{
			cout << "> ";
        }
		    
		string rawin;
		getline(cin, rawin);
		string trin = toLowercase(trimStr(rawin));

		if (commentMode)
		{
			if (trin == "" || trin == "exit")
			{
				commentMode = false;
				if (trin == "exit")
				{
					cout << "\033[F\r  " << trin << " -- exited comment mode" << endl;
				}
				else
				{

					cout << "\033[F\r  " << "-- exited comment mode" << endl;
				}

				continue;
			}

			cout << "\033[F\r/ " << trin << "   " << endl;
			continue;
		}

		if (trin == "exit")
		{
			cout << "\033[F\r  " << trin << " -- sucess" << endl;
			return 0;
		}

		if (trin == "")
		{
			cout << "\033[F\r  " << endl;
			continue;
		}

		if (trin.substr(0, 2) == "//")
		{
			string comment = trimStr(trin.substr(2, trin.length()));
			commentMode = true;

			int lenDiff = rawin.length() - 2 - comment.length();

			string fillSpaces = "";
			for (int i = lenDiff + 1; i >= 0; i--)
			{
				fillSpaces = fillSpaces + " ";
			}

			if (comment == "")
			{
				cout << "\033[F\r/  " << fillSpaces;
				continue;
			}

			cout << "\033[F\r/ " << comment << fillSpaces  << endl;
			continue;
		}

		string findOperator = "";

		for (size_t i = 0; i < sizeof(validOperators) / sizeof(*validOperators); i++)
		{
			if (findOperator != "") continue;
			if (trin.find(validOperators[i]) != string::npos) findOperator = validOperators[i];
		}

		bool secondNumberExists = false;
		float numbers[2];
		int operatorFindIndex = trin.find(findOperator);

		try
		{
			if (findOperator == "")
			{
				numbers[0] = parseToNumber(trimStr(trin));
			}
			else
			{
				numbers[1] = parseToNumber(trimStr(trin.substr(operatorFindIndex + 1, trin.length() - operatorFindIndex)));
				secondNumberExists = true;
				// we want numbers[0] to throw after numbers[1], cuz then we catch it and check if it should calc on last result, future me dont flip these two around this is important
				numbers[0] = parseToNumber(trimStr(trin.substr(0, operatorFindIndex)));
			}
		}
		catch (exception& err)
		{
			string errMsg = err.what();

			if (secondNumberExists)
            {
                if (lastResultExists)
                {
				    numbers[0] = lastResult;
                }
                else
                {
					cout << "\033[F\r! " << trin << " [ERR: NO HISTORY]" << endl;
				    continue;
                }
			}
			else
			{
				if (errMsg == "stof argument out of range")
				{
					cout << "\033[F\r! " << trin << " [ERR: OUT OF RANGE]" << endl;
				    continue;
				}

                cout << "\033[F\r! " << trin << " [ERR]" << endl;
				continue;
			}
		}

		float out;

		if (findOperator == "")
		{
			lastResult = numbers[0];
            lastResultExists = true;

			cout << "\033[F\r  " << numbers[0] << " = " << numbers[0] << endl;

			continue;
		}
		
        if (findOperator == "+")
		{
			out = numbers[0] + numbers[1];
		}
		else if (findOperator == "-")
		{
			out = numbers[0] - numbers[1];
		}
		else if (findOperator == "*")
		{
			out = numbers[0] * numbers[1];
		}
		else if (findOperator == "/")
		{
			out = numbers[0] / numbers[1];
		}

		lastResult = out;
        lastResultExists = true;

		cout << "\033[F\r  " << numbers[0] << " " << findOperator << " " << numbers[1] << " = " << out << endl;
	}

	return 0;
}
