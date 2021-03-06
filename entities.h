/*
	Entity/Delimiter Header
	Author: Greg Frazier
	March 5, 2009
*/
// All the irregular HTML 3.2 Entities
// 95 of them

const char delimiters[] = { '<', '>', '&', 32, '\0', 0xa, 0xd, '\0'};
const char entdelim[] = { '<', '>', ';', 32, '\0', '\0'};
const char attdelim[] = { '>', '=', 32, '\0', 0xa, 0xd, '\0'};

// Literal Entities
// These are entities in the Latin character code set.
// These are characters technically not "allowed" in markup as a literal character, they need to be converted to these entities first.
// Example: &nbsp; adds a SPACE (chr(13)) to the rendered webpage.
const char *entities[] = {
							"lt",       // Less than Sign <						
							"gt",       // Greater than Sign >
							"amp",		// Ampersand &
							"nbsp",     // no-break space 
							"iexcl",    // inverted exclamation mark 
							"cent",     // cent sign 
							"pound",    // pound sterling sign 
							"curren",   // general currency sign 
							"yen",      // yen sign 
							"brvbar",   // broken (vertical) bar 
							"sect",     // section sign 
							"uml",      // umlaut (dieresis) 
							"copy",     // copyright sign 
							"ordf",     // ordinal indicator, feminine 
							"laquo",    // angle quotation mark, left 
							"not",      // not sign 
							"shy",      // soft hyphen 
							"reg",      // registered sign 
							"macr",     // macron 
							"deg",      // degree sign 
							"plusmn",   // plus-or-minus sign 
							"sup2",     // superscript two 
							"sup3",     // superscript three 
							"acute",    // acute accent 
							"micro",    // micro sign 
							"para",     // pilcrow (paragraph sign) 
							"middot",   // middle dot 
							"cedil",    // cedilla 
							"sup1",     // superscript one 
							"ordm",     // ordinal indicator, masculine 
							"raquo",    // angle quotation mark, right 
							"frac14",   // fraction one-quarter 
							"frac12",   // fraction one-half 
							"frac34",   // fraction three-quarters 
							"iquest",   // inverted question mark 
							"Agrave",   // capital A, grave accent 
							"Aacute",   // capital A, acute accent 
							"Acirc",    // capital A, circumflex accent 
							"Atilde",   // capital A, tilde 
							"Auml",     // capital A, dieresis or umlaut mark 
							"Aring",    // capital A, ring 
							"AElig",    // capital AE diphthong (ligature) 
							"Ccedil",   // capital C, cedilla 
							"Egrave",   // capital E, grave accent 
							"Eacute",   // capital E, acute accent 
							"Ecirc",    // capital E, circumflex accent 
							"Euml",     // capital E, dieresis or umlaut mark 
							"Igrave",   // capital I, grave accent 
							"Iacute",   // capital I, acute accent 
							"Icirc",    // capital I, circumflex accent 
							"Iuml",     // capital I, dieresis or umlaut mark 
							"ETH",      // capital Eth, Icelandic 
							"Ntilde",   // capital N, tilde 
							"Ograve",   // capital O, grave accent 
							"Oacute",   // capital O, acute accent 
							"Ocirc",    // capital O, circumflex accent 
							"Otilde",   // capital O, tilde 
							"Ouml",     // capital O, dieresis or umlaut mark 
							"times",    // multiply sign 
							"Oslash",   // capital O, slash 
							"Ugrave",   // capital U, grave accent 
							"Uacute",   // capital U, acute accent 
							"Ucirc",    // capital U, circumflex accent 
							"Uuml",     // capital U, dieresis or umlaut mark 
							"Yacute",   // capital Y, acute accent 
							"THORN",    // capital THORN, Icelandic 
							"szlig",    // small sharp s, German (sz ligature) 
							"agrave",   // small a, grave accent 
							"aacute",   // small a, acute accent 
							"acirc",    // small a, circumflex accent 
							"atilde",   // small a, tilde 
							"auml",     // small a, dieresis or umlaut mark 
							"aring",    // small a, ring 
							"aelig",    // small ae diphthong (ligature) 
							"ccedil",   // small c, cedilla 
							"egrave",   // small e, grave accent 
							"eacute",   // small e, acute accent 
							"ecirc",    // small e, circumflex accent 
							"euml",     // small e, dieresis or umlaut mark 
							"igrave",   // small i, grave accent 
							"iacute",   // small i, acute accent 
							"icirc",    // small i, circumflex accent 
							"iuml",     // small i, dieresis or umlaut mark 
							"eth",      // small eth, Icelandic 
							"ntilde",   // small n, tilde 
							"ograve",   // small o, grave accent 
							"oacute",   // small o, acute accent 
							"ocirc",    // small o, circumflex accent 
							"otilde",   // small o, tilde 
							"ouml",     // small o, dieresis or umlaut mark 
							"divide",   // divide sign 
							"oslash",   // small o, slash 
							"ugrave",   // small u, grave accent 
							"uacute",   // small u, acute accent 
							"ucirc",    // small u, circumflex accent 
							"uuml",     // small u, dieresis or umlaut mark 
							"yacute",   // small y, acute accent 
							"thorn",    // small thorn, Icelandic 
							"yuml",     // small y, dieresis or umlaut mark 
							""
							};

// These are the decimal translations to the above entities, some editors use these instead
// Most are not compatable with Strict ASCII (but do work with extended), 
// so if a U with an umlat (?) is asked, ignore and just type a U
// Example in Markup: &#160; is the same as &nbsp;
const char *entities_num[] = {
							"60",  // Lessthan
							"62",  // greaterthan
							"38",  // &
							"160", // no-break space 
							"161", // inverted exclamation mark 
							"162", // cent sign 
							"163", // pound sterling sign 
							"164", // general currency sign 
							"165", // yen sign 
							"166", // broken (vertical) bar 
							"167", // section sign 
							"168", // umlaut (dieresis) 
							"169", // copyright sign 
							"170", // ordinal indicator, feminine 
							"171", // angle quotation mark, left 
							"172", // not sign 
							"173", // soft hyphen 
							"174", // registered sign 
							"175", // macron 
							"176", // degree sign 
							"177", // plus-or-minus sign 
							"178", // superscript two 
							"179", // superscript three 
							"180", // acute accent 
							"181", // micro sign 
							"182", // pilcrow (paragraph sign) 
							"183", // middle dot 
							"184", // cedilla 
							"185", // superscript one 
							"186", // ordinal indicator, masculine 
							"187", // angle quotation mark, right 
							"188", // fraction one-quarter 
							"189", // fraction one-half 
							"190", // fraction three-quarters 
							"191", // inverted question mark 
							"192", // capital A, grave accent 
							"193", // capital A, acute accent 
							"194", // capital A, circumflex accent 
							"195", // capital A, tilde 
							"196", // capital A, dieresis or umlaut mark 
							"197", // capital A, ring 
							"198", // capital AE diphthong (ligature) 
							"199", // capital C, cedilla 
							"200", // capital E, grave accent 
							"201", // capital E, acute accent 
							"202", // capital E, circumflex accent 
							"203", // capital E, dieresis or umlaut mark 
							"204", // capital I, grave accent 
							"205", // capital I, acute accent 
							"206", // capital I, circumflex accent 
							"207", // capital I, dieresis or umlaut mark 
							"208", // capital Eth, Icelandic 
							"209", // capital N, tilde 
							"210", // capital O, grave accent 
							"211", // capital O, acute accent 
							"212", // capital O, circumflex accent 
							"213", // capital O, tilde 
							"214", // capital O, dieresis or umlaut mark 
							"215", // multiply sign 
							"216", // capital O, slash 
							"217", // capital U, grave accent 
							"218", // capital U, acute accent 
							"219", // capital U, circumflex accent 
							"220", // capital U, dieresis or umlaut mark 
							"221", // capital Y, acute accent 
							"222", // capital THORN, Icelandic 
							"223", // small sharp s, German (sz ligature) 
							"224", // small a, grave accent 
							"225", // small a, acute accent 
							"226", // small a, circumflex accent 
							"227", // small a, tilde 
							"228", // small a, dieresis or umlaut mark 
							"229", // small a, ring 
							"230", // small ae diphthong (ligature) 
							"231", // small c, cedilla 
							"232", // small e, grave accent 
							"233", // small e, acute accent 
							"234", // small e, circumflex accent 
							"235", // small e, dieresis or umlaut mark 
							"236", // small i, grave accent 
							"237", // small i, acute accent 
							"238", // small i, circumflex accent 
							"239", // small i, dieresis or umlaut mark 
							"240", // small eth, Icelandic 
							"241", // small n, tilde 
							"242", // small o, grave accent 
							"243", // small o, acute accent 
							"244", // small o, circumflex accent 
							"245", // small o, tilde 
							"246", // small o, dieresis or umlaut mark 
							"247", // divide sign 
							"248", // small o, slash 
							"249", // small u, grave accent 
							"250", // small u, acute accent 
							"251", // small u, circumflex accent 
							"252", // small u, dieresis or umlaut mark 
							"253", // small y, acute accent 
							"254", // small thorn, Icelandic 
							"255", // small y, dieresis or umlaut mark 
							""
							};

const char *entities_trans[] = {
							"<",
							">",
							"&",
							" ", //160
							"¡",
							"?",
							"£",
							"?",
							"¥",
							"|",
							"?",
							"\"",
							"?",
							"ª", //170
							"«",
							"¬",
							"-",
							"?",
							"?",
							"º",
							"±",
							"²",
							"?",
							"'", //180
							"µ",
							"?",
							"·",
							"?",
							"?",
							"?",
							"»",
							"¼",
							"½",
							"?", //190
							"¿",
							"A",
							"A",
							"A",
							"A",
							"Ä",
							"Å",
							"Æ",
							"Ç",
							"E", //200
							"É",
							"E",
							"E",
							"I",
							"I",
							"I",
							"I",
							"D",
							"Ñ",
							"O", //210
							"O",
							"O",
							"O",
							"Ö",
							"x",
							"φ",
							"U",
							"U",
							"U",
							"Ü", //220
							"Y",
							"?",
							"ß",
							"à",
							"a",
							"â",
							"a",
							"ä",
							"å",
							"æ", //230
							"c",
							"è",
							"é",
							"ê",
							"ë",
							"ì",
							"í",
							"i",
							"i",
							"?", //240
							"ñ",
							"o",
							"o",
							"o",
							"o",
							"o",
							"?",
							"φ",
							"u",
							"u", //250
							"u",
							"u",
							"y",
							"?",
							"y", //255
							""
							};