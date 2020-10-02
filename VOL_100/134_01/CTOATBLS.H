/* CTOATBLS.C */
/*	Include file for tables in CTOATBLS.CSM		*/

/*	Operation types in opctabl () 	*/

#define OP_SIMPLE	1	/* Simple one-byte operation */
#define OP_LXI		2	/* LXI operation */
#define OP_RPSP		3	/* Register pair in bits 5-4, 11 means SP */
#define OP_REG53	4	/* Register number in bits 5-3 */
#define OP_MVI		5	/* MVI operation */
#define OP_DATA1	6	/* LDA/STA; follower is addr of byte datum */
#define OP_DATA2	7	/* LHLD/SHLD; follower is addr of word datum */
#define OP_MOV		8	/* MOV operation */
#define OP_ARITH	9	/* Arithmetic operation */
#define OP_CONRET	10	/* Conditional return */
#define OP_RPPSW	11	/* Register pair in bits 5-4, 11 means PSW */
#define OP_CONTRA	12	/* Conditional transfer */
#define OP_TRANS	13	/* Unconditional transfer */
#define OP_IARITH	14	/* Arithmetic with immediate operand */
#define OP_RST		15	/* Restart operation */
#define OP_IO		16	/* I/O operation */
#define OP_RST6		17	/* Special case for RST 6 */
#define OP_WHAT		18	/* Unknown operation */

/*	Structure of an opcode table entry	*/

struct opc_entry {
	char opc_mask;		/* Comparison mask */
	char opc_val;		/* Opcode value */
	char opc_name [5];	/* Opcode name */
	char opc_type;		/* Opcode type */
	};

/*	Structure of a ccc address table entry	*/

struct ccc_entry {
	unsigned ccc_addr;	/* Address in C.CCC of the entrypoint */
	char ccc_flags;		/* Flag bits (see below) */
	char ccc_name [7];	/* Entry point name */
	};

/*	Flag bits in a C.CCC address table entry	*/

#define CCC_CODE	0x80	/* Procedure entry point */
#define CCC_DATA	0x40	/* Data cell in low memory */
#define CCC_NMAC	0x20	/* Symbol not in macro library */
-4, 11 means PSW */
#define OP_CONTRA	12	/* Conditional transfer */
#define OP_TRANS	13	/* Unco