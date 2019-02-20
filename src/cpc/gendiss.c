
char Diss_GetHexDigitAsChar(unsigned char HexDigit)
{
    HexDigit &=0x0f;
    if ((HexDigit>=0) && (HexDigit<=9))
        return '0'+HexDigit;
    return 'A'+(HexDigit-10);
}


char *Diss_WriteHexByte(char *pString, unsigned char Value)
{
	unsigned char HexDigit;

	/* write identifier to indicate hex value */
	pString[0] = '#';

	/* write high digit */
	HexDigit = Value>>4;

	pString[1] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value & 0x0f;

	pString[2] = Diss_GetHexDigitAsChar(HexDigit);

    pString+=3;

    return pString;
}


char *Diss_WriteHexWord(char *pString, unsigned char Value)
{
	unsigned char HexDigit;

	/* write identifier to indicate hex value */
	pString[0] = '#';

	HexDigit = Value>>12;

	pString[1] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value>>8;

	pString[2] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value>>4;

	pString[3] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value;

	pString[4] = Diss_GetHexDigitAsChar(HexDigit);

    pString+=5;

    return pString;
}


static char *Diss_char(char *pString, const char ch)
{
        pString[0] = ch;
        ++pString;
        return pString;
}

char *Diss_space(char *pString)
{
	return Diss_char(pString,' ');
}

char *Diss_comma(char *pString)
{
	return Diss_char(pString,',');
}

char *Diss_colon(char *pString)
{
	return Diss_char(pString,':');
}

char *Diss_endstring(char *pString)
{
    pString[0] = '\0';
    ++pString;
    return pString;
}

char *Diss_strcat(char *pString, const char *pToken)
{
    int nTokenLength = strlen(pToken);
    strncpy(pString, pToken, nTokenLength);
    pString += nTokenLength;
    return pString;
}
