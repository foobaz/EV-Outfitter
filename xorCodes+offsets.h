const short offsets[6][16] =
{
	{1035, 1036, 1037, 1039, 1066,0,0,0,0,0,0,0,0,0,0,0},
	{1020,1025,1030,1031,1032,1033,1034,1038,1040,1041,1042,1043,1050,1052,1053,1054},
	{2141,2143,2146,2147,2156,2157,2160,2165,0,0,0,0,0,0,0,0},
	{2142,2144,2145,2148,2152,2153,2154,2155,2161,2162,2163,0,0,0,0,0},
	{2149,2150,2151,2158,2159,2164,2213,2214,2215,2222,2223,2228,0,0,0,0},
	{2208,2209,2216,2217,2218,2225,2227,0,0,0,0,0,0,0,0,0}
};

const short xorCodes[6][16] =
{
	{0x538C, 0xD867, 0xAC94, 0xD56C, 0x2760},
	// thrust  engine  RCS up. esc.pod auto-eject
	
	{0xDCF1, 0x3234, 0xE7FB, 0xD1AC, 0x1804, 0x2E74, 0x281C, 0x69B8, 0x96CB, 0x2AB4, 0xABD5, 0x574C, 0x2AF2, 0xD70D, 0xA714, 0x6B16},
	//crg.pods m.exp sh.cap sh.boost |-L-----armor------H-|fuelTanks d.scanner IFF  aft.burn mis.jam reg.map cloaks ramscops autofueller
	
	{0x4914, 0xB6EC, 0x1815, 0x34CC, 0xA714, 0x5194, 0xE4B0, 0x55D4},
//	 lasers protons l.turr. p.turr. rear L.t  swivel fusion  heavy fusion
	
	{0x57F9, 0xE80A, 0xCB34, 0x286F, 0x66F3, 0xCF74, 0x9B0C, 0x308C, 0xD3B4, 0x1DF3, 0x2C4C,0,0,0,0,0},
//	  neut.  torps  missiles cannons rockets s.bombs jav.pods p.beam seekers tractor flare launchers
	
	/*bays*/ {0x4D54, 0xD9B0, 0xB2AC, 0x5B6F, 0xAE6C, 0x220D,
//			patrolShips mantas lightngs gunboats hawks  aliens
	/*ships*/ 0x4F54, 0xA1E8, 0xB0AC, 0x23A7, 0xAC6C, 0x9D15},
	
	{0x6272, 0xCD34, 0x5E3B, 0xD174, 0xE244, 0xD5B4, 0x2A4C}
//	  torps missiles rockets s.bombs javelins drones flares
};