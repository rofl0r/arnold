#ifndef __GEN_DISS_HEADER_INCLUDED__
#define __GEN_DISS_HEADER_INCLUDED__

char *Diss_WriteHexByte(char *pString, unsigned char Value);
char *Diss_space(char *pString);
char *Diss_comma(char *pString);
char *Diss_colon(char *pString);
char *Diss_endstring(char *pString);
char *Diss_strcat(char *pString, const char *pToken);

#endif