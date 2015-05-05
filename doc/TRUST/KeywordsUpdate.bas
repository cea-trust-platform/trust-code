Sub UpdateKeywords()
' Read the dictionary
ThisDoc=ThisComponent
ThisText=ThisDoc.Text
TheCursor=ThisText.createTextCursor

file="Keywords.txt"
' File syntax:
' EnglishKeyword|MotCleFrancais

iCount = FreeFile()
Dim sFrench as string
Dim sEnglish as string
Dim sLine as string

Open file for Input as #iCount
Do while NOT EOF(iCount)
	Line Input #iCount; sLine
	s=split(sLine,"|",2)
	sEnglish=s(0)
	sFrench=s(1)
	if (sEnglish<>"") then 
		FandR = ThisDoc.createReplaceDescriptor 
		With FandR 
		.SearchString = " " & sFrench & " "
		.ReplaceString = " " & sEnglish & " "
		End with 
		Found = ThisDoc.findFirst(FandR)
		if (Not isNull(Found)) then 
			MsgBox "Replacing <" & FandR.SearchString & "> by <" & FandR.ReplaceString & "> ?"
			While Not isNull(Found) 
				Found.setString(FandR.ReplaceString)
			  	Found = ThisDoc.FindNext(Found.End,FandR)
			Wend 
		endif
	endif
	Loop
Close #iCount
End Sub

