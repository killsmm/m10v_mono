#include <stdio.h>
#include "../include/im_tuning_buffer.h"
#include "../include/im_tuning_cmd.h"

#if 0
#define kCmdMaxSize	(128)
unsigned char gBufCommand[kCmdMaxSize];
#define kShowStringSize	 (kCmdMaxSize+64)
unsigned char gBufShow[kShowStringSize];
#define kMaxARGC	(10)
unsigned char *gArgv[kMaxARGC];
unsigned long gArgC = 0;
#define kBinDataSize	(64)
unsigned char gBinCmd;
unsigned char gBinData[kBinDataSize];
unsigned long gBinDataSize;
unsigned char gBinSID;
#endif

#define kReadNextByte	 {				 \
	if (p >= endp) goto  _too_short_err; \
 	c = *p++; 							 \
 	sum +=c;							 \
	}
#define kReadPreByte    {               \
    sum -= c;                           \
    c = *--p;                           \
    }
int bincmd_parse(unsigned char *in_str, unsigned long in_len, unsigned char *out_cmd, unsigned char *out_data, unsigned long *out_datalen, unsigned char *out_sid, unsigned long max_outdata)
{
	unsigned char *p, *endp, c, c2, sid;
	unsigned long sizecmd, data_cnt;
	unsigned char cmd, checksum;
	unsigned int sum=0;
	sizecmd = 0xFFFFFFFF;



	sum = 0;
	p = in_str;
	endp = p + in_len;
	// Check Head & SID
	kReadNextByte;
	if ((c & 0xF0) != kBinCmd_Start) \
		return kBinCmd_Err_Cmd;		\
	sid = c & 0xF;

	// Check command code
	kReadNextByte
	if ((c & 0xF0) != kBinCmd_CmdID)
		return kBinCmd_Err_Cmd;
	cmd = c & 0xF;
	kReadNextByte
	if ((c & 0xF0) != kBinCmd_CmdID)
		return kBinCmd_Err_Cmd;
	cmd = (cmd << 4) + (c & 0xF);

	kReadNextByte
	if ((c & 0xF0) == kBinCmd_Size)
	{
		sizecmd = c & 0xF;
		while (1)
		{
			kReadNextByte
			if ((c & 0xF0) == kBinCmd_Size)
			{
				sizecmd = sizecmd << 4;
				sizecmd += (c & 0xF);
			}
			else {
                kReadPreByte
				break;
			}
		}
	}
	else{
        kReadPreByte
	}

	data_cnt = 0;
	while (1)
	{
		kReadNextByte
		if ((c & 0xF0) != kBinCmd_Data)
        {
            kReadPreByte
            break;
        }
		if (data_cnt >= max_outdata)
			return kBinCmd_Err_DataBufOver;
		c2 = c;
		kReadNextByte
		if ((( c) & 0xF0) == kBinCmd_Data)
		{
			c = c2 << 4 | (c & 0xf);
			out_data[data_cnt++] = c;
		}
		if (data_cnt >= max_outdata)
			return kBinCmd_Err_DataBufOver;
	}

	if (sizecmd != 0xFFFFFFFF && sizecmd != data_cnt)
	{
		return kBinCmd_Err_SizeNotMatch;
	}
	// Tail
	c = *(p+1);
	if ((c & 0xF0) == kBinCmd_End)
	{
		checksum = c & 0xF;
		//sum = sum - c;
		sum = ((sum >> 4)&0xF) + (sum & 0xF);
		sum = sum & 0xF;
		if (checksum != sum)
			return kBinCmd_Err_CheckSum;

	}
	else {
	if (data_cnt >= max_outdata)
		return kBinCmd_Err_NO_Tail;

	}

	*out_cmd = cmd;
	*out_datalen = data_cnt;
	*out_sid = sid;

	return 0;
_too_short_err:
	return kBinCmd_Err_CmdTooShort;
#undef kReadNextByte
}



int _isQuote(char c);
int _isEscape(char c);
int _isEscape(char c);
int _isWhitespace(char c);

#define TRUE (1)
#define FALSE (0)


// current state
enum State {
	InArg,      // currently scanning an argument
	InArgQuote, // currently scanning an argument (which started with quotes)
	OutOfArg    // currently not scanning an argument
};

int asciicmd_parse(unsigned char *in_args, long in_len, unsigned char **out_argv, unsigned long *out_argc)
{


	int currentState = OutOfArg;
	unsigned char *currstr = NULL;

	char currentQuoteChar = '\0'; // to distinguish between ' and " quotations
								  // this allows to use "foo'bar"
	int argc = 0;
	unsigned char c, *p, *endp;

	p = in_args;
	endp = p + in_len;
	while (p < endp)
	{ // iterate char by char
		c = *p++;
		if (c == '\0' || c== '\r' || c== '\n')
			break;
		if (_isQuote(c)) {
			switch (currentState) {
			case OutOfArg:
				//				currentArg.str(std::string());
				currstr = p - 1;
				out_argv[argc] = currstr;
			case InArg:
				currentState = InArgQuote;
				currentQuoteChar = c;
				break;

			case InArgQuote:
				if (c == currentQuoteChar) {
					currentState = InArg;
				}
				else {
					//					currentArg << c;
				}
				break;
			}

		}
		else if (_isWhitespace(c)) {
			switch (currentState) {
			case InArg:
				//				oargs.push_back(currentArg.str());
				*(p - 1) = '\0';
				argc++;
				currentState = OutOfArg;
				break;
			case InArgQuote:
				//				currentArg << c;
				break;
			case OutOfArg:
				// nothing
				break;
			}
		}
		else if (_isEscape(c)) {
			switch (currentState) {
			case OutOfArg:
				//				currentArg.str(std::string());
				currstr = p - 1;
				out_argv[argc] = currstr;
				currentState = InArg;
			case InArg:
			case InArgQuote:
				if (!(p < endp))
				{

				}
				else
				{

					// Windows only escapes the " character.
					// Every other character is just printed and the \ is added itself.

					if (*(p + 1) == '\"') {
						// skip next
						p++;// only ignore \ when next char is "
					}

				}
				break;
			}
		}
		else {
			switch (currentState) {
			case InArg:
			case InArgQuote:
				//			currentArg << c;
				break;

			case OutOfArg:

				//				currentArg.str(std::string());
				//				currentArg << c;
				currstr = p - 1;
				out_argv[argc] = currstr;

				currentState = InArg;
				break;
			}
		}
	}

	if (currentState == InArg) {
	//oargs.push_back(currentArg.str());
		*(p) = '\0';

		argc++;
	}
	else if (currentState == InArgQuote) {
		*(p) = '\0';
		argc++;
//		throw(std::runtime_error("Starting quote has no ending quote."));
	}

	*out_argc = argc;
	return argc;
}

int _isQuote(char c)
{
	if (c == '\"')
		return TRUE;
	else if (c == '\'')
		return TRUE;

	return FALSE;
}

int _isEscape(char c)
{
	if (c == '\\')
		return TRUE;

	return FALSE;
}

int _isWhitespace(char c)
{
	if (c == ' ')
		return TRUE;
	else if (c == '\t')
		return TRUE;

	return FALSE;
}
