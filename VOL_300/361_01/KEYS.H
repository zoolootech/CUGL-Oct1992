
/* Keys.h ---> Non-ASCII ScanCodes | 128 for UnSigned Byte Representation  */

#define ALT_9           128
#define ALT_0           129
#define ALT_DASH        130
#define ALT_EQUALS      131
#define CTL_PGUP        132
#define ALT_BS		142
#define SHIFT_HT        143
#define ALT_Q           144
#define ALT_W           145
#define ALT_E           146
#define ALT_R           147
#define ALT_T           148
#define ALT_Y           149
#define ALT_U           150
#define ALT_I           151
#define ALT_O           152
#define ALT_P           153
#define ALT_LBRACE      154
#define ALT_RBRACE      155
#define ALT_RTN         156
#define EK157		157
#define ALT_A           158
#define ALT_S           159
#define ALT_D           160
#define ALT_F           161
#define ALT_G           162
#define ALT_H           163
#define ALT_J           164
#define ALT_K           165
#define ALT_L           166
#define ALT_SEMI        167
#define ALT_QUOTE       168
#define ALT_TILDE       169
#define EK170           170
#define ALT_BKSL        171
#define ALT_Z           172
#define ALT_X           173
#define ALT_C           174
#define ALT_V           175
#define ALT_B           176
#define ALT_N           177
#define ALT_M           178
#define ALT_COMMA       179
#define ALT_DOT         180
#define ALT_QUERY       181
#define EK182           182
#define ALT_STAR        183
#define EK184           184
#define EK185           185
#define EK186           186
#define F1              187
#define F2              188
#define F3              189
#define F4              190
#define F5              191
#define F6              192
#define F7              193
#define F8              194
#define F9              195
#define F10             196
#define EK197           197
#define EK198           198
#define HOME            199
#define UP              200
#define PGUP            201
#define ALT_MINUS       202
#define BWD             203
#define EK204           204
#define FWD             205
#define ALT_PLUS        206
#define END             207
#define DN              208
#define PGDN            209
#define INS             210
#define DEL_KEY         211
#define SHIFT_F1        212
#define SHIFT_F2        213
#define SHIFT_F3        214
#define SHIFT_F4        215
#define SHIFT_F5        216
#define SHIFT_F6        217
#define SHIFT_F7        218
#define SHIFT_F8        219
#define SHIFT_F9        220
#define SHIFT_F10       221
#define CTL_F1          222
#define CTL_F2          223
#define CTL_F3          224
#define CTL_F4          225
#define CTL_F5          226
#define CTL_F6          227
#define CTL_F7          228
#define CTL_F8          229
#define CTL_F9          230
#define CTL_F10         231
#define ALT_F1          232
#define ALT_F2          233
#define ALT_F3          234
#define ALT_F4          235
#define ALT_F5          236
#define ALT_F6          237
#define ALT_F7          238
#define ALT_F8          239
#define ALT_F9          240
#define ALT_F10         241
#define CTL_BWD         243
#define EK242           242
#define EK243           243
#define CTL_FWD         244
#define CTL_END         245
#define CTL_PGDN        246
#define CTL_HOME        247
#define ALT_1           248
#define ALT_2           249
#define ALT_3           250
#define ALT_4           251
#define ALT_5           252
#define ALT_6           253
#define ALT_7           254
#define ALT_8           255

/* In their finite wisdom, IBM put support for the AT's "Extended Keycode" in
 * the BIOS and then failed to make it available to DOS.  PC Power Tools has
 * NewKeys.Com which "Patches" DOS to access INTR 16H via functions 10H-12H
 * (instead of 0-2) thereby picking up 31 new ScanCodes for the 101 Key "Big
 * Keyboard" starting at 133.  These 10 are unique when ORed w/ 128:
 */
#define F11             133
#define F12             134
#define SHIFT_F11       135
#define SHIFT_F12       136
#define CTL_F11         137
#define CTL_F12         138
#define ALT_F11         139
#define ALT_F12         140
#define CTL_UP          141
#define ALT_BWD         155
#define ALT_FWD         157

/* These NewKeys Conflict w/ "Normal" KeyCodes */
#define CTL_DASH        142		/* ALT_BS */
#define CTL_5		143		/* SHIFT_HT */
#define CTL_PLUS	144		/* ALT_Q */
#define CTL_DN		145		/* ALT_W */
#define CTL_INS		146		/* ALT_E */
#define CTL_DEL_KEY	147		/* ALT_R */
#define CTL_HT		148		/* ALT_T */
#define CTL_SL		149		/* ALT_Y */
#define CTL_STAR	150		/* ALT_U */
#define ALT_HOME	151		/* ALT_I */
#define ALT_UP		152		/* ALT_O */
#define ALT_PGUP	153		/* ALT_P */
#define ALT_END		159		/* ALT_S */
#define ALT_DN		160		/* ALT_D */
#define ALT_PGDN	161		/* ALT_F */
#define ALT_INS		162		/* ALT_G */
#define ALT_DEL_KEY	163		/* ALT_H */
#define ALT_SL		164		/* ALT_J */
#define ALT_HT		165		/* ALT_K */
#define ALT_ENTER	166		/* ALT_L */

