	AREA ADD,CODE,READONLY
	ENTRY
	LDR R0,=0x40000000
	LDR R1,[R0]
	LDR R2,[R0,#4]
	SUBS R3,R1,R2
	STR R3,[R0,#8]
L1 	B L1
 	END