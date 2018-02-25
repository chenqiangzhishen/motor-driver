#pragma once
#include "stdafx.h"
#include "Printer.h"

//
// Printer Test
//
void PrintTest() {
	char szPrinter[80];
	char *szDevice, *szDriver, *szOutput;
	TEXTMETRIC tm;
	HDC hdcPrint; // define a hdc

	// define a printer job
	static DOCINFO di = {sizeof(DOCINFO), "printer", NULL};
	// get printer info
	GetProfileString("windows", "device", ",,,", szPrinter, 80);
	// resolve printer
	if (NULL != (szDevice = strtok(szPrinter, ",")) && NULL != (szDriver = strtok(NULL, ",")) &&
		NULL != (szOutput = strtok(NULL, ",")))
		if ((hdcPrint = CreateDC(szDriver, szDevice, szOutput, NULL)) != 0)
		{
			// get text font info
			GetTextMetrics(hdcPrint, &tm);
			if (StartDoc(hdcPrint, &di) > 0) //start a job
			{
				StartPage(hdcPrint); // run paper
				SaveDC(hdcPrint);
				TCHAR company[] = "ª∂”≠π‚¡Ÿ£°";
				char nce[] = "welcome to NCE!";
				char product[] = "NCE8900K...";
				TextOut(hdcPrint, 1, 1, company, ARRAYSIZE(company));
				TextOut(hdcPrint, 1, 1+tm.tmHeight, nce, sizeof nce);
				TextOut(hdcPrint, 1, 1+2*tm.tmHeight, product, sizeof product);

				RestoreDC(hdcPrint, -1); // restore DC
				EndPage(hdcPrint); // stop running paper
				EndDoc(hdcPrint); // stop a job
				printf("print finished.\n");
			}
			//release DC
			DeleteDC(hdcPrint);
		}
		else
		{
			printf("can not find printer\n");
			return;
		}
}
