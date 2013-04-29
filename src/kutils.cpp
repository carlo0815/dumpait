#include "define.h"

BOOL is_hex_str(char* str)
{
	char* temp = str;
	while(*temp) {
		switch(*temp) 
		{
		case '0': break;
		case '1': break;
		case '2': break;
		case '3': break;
		case '4': break;
		case '5': break;
		case '6': break;
		case '7': break;
		case '8': break;
		case '9': break;
		case 'a': break;
		case 'b': break;
		case 'c': break;
		case 'd': break;
		case 'e': break;
		case 'f': break;
		case 'A': break;
		case 'B': break;
		case 'C': break;
		case 'D': break;
		case 'E': break;
		case 'F': break;
		default:  return FALSE;
		}
		++temp;
	}
	return TRUE;
}

int htoi(char *str)
{
	int   degit	= 0;
	char* hexa	= str;

	while(*hexa) {
		degit *= 16;
		switch(*hexa) 
		{
		case '0': 
		case '1': 
		case '2': 
		case '3': 
		case '4': 
		case '5': 
		case '6': 
		case '7': 
		case '8': 
		case '9': 
			degit += *hexa - '0';
			break;
		case 'a': 
		case 'b': 
		case 'c': 
		case 'd': 
		case 'e': 
		case 'f': 
			degit += *hexa - 'a' + 10;
			break;
		case 'A': 
		case 'B': 
		case 'C': 
		case 'D': 
		case 'E': 
		case 'F': 
			degit += *hexa - 'A' + 10;
			break;
		}
		++hexa;
	}
	return degit;
}

