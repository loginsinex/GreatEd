/*
	A NES Super Mario Bros Level Editor [public version]
	Copyright (C) 2025 ALXR aka loginsin
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

NES_INSTRUCTION_MAP		l_mCode =
{
	std::make_pair( niiBRK, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocBRK_IMPL ) } ) ),
	std::make_pair( niiPHP, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocPHP_IMPL ) } ) ),
	std::make_pair( niiCLC, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocCLC_IMPL ) } ) ),
	std::make_pair( niiPLP, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocPLP_IMPL ) } ) ),
	std::make_pair( niiSEC, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocSEC_IMPL ) } ) ),
	std::make_pair( niiRTI, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocRTI_IMPL ) } ) ),
	std::make_pair( niiPHA, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocPHA_IMPL ) } ) ),
	std::make_pair( niiCLI, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocCLI_IMPL ) } ) ),
	std::make_pair( niiRTS, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocRTS_IMPL ) } ) ),
	std::make_pair( niiPLA, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocPLA_IMPL ) } ) ),
	std::make_pair( niiSEI, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocSEI_IMPL ) } ) ),
	std::make_pair( niiDEY, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocDEY_IMPL ) } ) ),
	std::make_pair( niiTXA, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocTXA_IMPL ) } ) ),
	std::make_pair( niiTYA, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocTYA_IMPL ) } ) ),
	std::make_pair( niiTXS, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocTXS_IMPL ) } ) ),
	std::make_pair( niiTAY, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocTAY_IMPL ) } ) ),
	std::make_pair( niiTAX, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocTAX_IMPL ) } ) ),
	std::make_pair( niiCLV, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocCLV_IMPL ) } ) ),
	std::make_pair( niiTSX, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocTSX_IMPL ) } ) ),
	std::make_pair( niiINY, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocINY_IMPL ) } ) ),
	std::make_pair( niiDEX, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocDEX_IMPL ) } ) ),
	std::make_pair( niiCLD, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocCLD_IMPL ) } ) ),
	std::make_pair( niiINX, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocINX_IMPL ) } ) ),
	std::make_pair( niiNOP, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocNOP_IMPL ) } ) ),
	std::make_pair( niiSED, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitIMPL, niocSED_IMPL ) } ) ),

	std::make_pair( niiBPL, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBPL_REL ) } ) ),
	std::make_pair( niiBMI, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBMI_REL ) } ) ),
	std::make_pair( niiBVC, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBVC_REL ) } ) ),
	std::make_pair( niiBVS, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBVS_REL ) } ) ),
	std::make_pair( niiBCC, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBCC_REL ) } ) ),
	std::make_pair( niiBCS, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBCS_REL ) } ) ),
	std::make_pair( niiBNE, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBNE_REL ) } ) ),
	std::make_pair( niiBEQ, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitREL, niocBEQ_REL ) } ) ),

	std::make_pair( niiJSR, NES_INSTRUCTION_CODE_MAP( { std::make_pair( nitABS, niocJSR_ABS ), std::make_pair( nitLABEL, niocJSR_ABS ) } ) ),
	std::make_pair( niiJMP, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitABS, niocJMP_ABS ),
			std::make_pair( nitLABEL, niocJMP_ABS ),
			std::make_pair( nitIND, niocJMP_IND )
		} ) ),

	std::make_pair( niiORA, NES_INSTRUCTION_CODE_MAP(
	{
		std::make_pair( nitX_IND, niocORA_X_IND ),
		std::make_pair( nitIND_Y, niocORA_IND_Y ),
		std::make_pair( nitZPG, niocORA_ZPG ),
		std::make_pair( nitZPG_X, niocORA_ZPG_X ),
		std::make_pair( nitIMM, niocORA_IMM ),
		std::make_pair( nitABS, niocORA_ABS ),
		std::make_pair( nitABSX, niocORA_ABS_X ),
		std::make_pair( nitABSY, niocORA_ABS_Y )
	} ) ),

	std::make_pair( niiAND, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocAND_X_IND ),
			std::make_pair( nitIND_Y, niocAND_IND_Y ),
			std::make_pair( nitZPG, niocAND_ZPG ),
			std::make_pair( nitZPG_X, niocAND_ZPG_X ),
			std::make_pair( nitIMM, niocAND_IMM ),
			std::make_pair( nitABS, niocAND_ABS ),
			std::make_pair( nitABSX, niocAND_ABS_X ),
			std::make_pair( nitABSY, niocAND_ABS_Y )
		} ) ),

	std::make_pair( niiEOR, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocEOR_X_IND ),
			std::make_pair( nitIND_Y, niocEOR_IND_Y ),
			std::make_pair( nitZPG, niocEOR_ZPG ),
			std::make_pair( nitZPG_X, niocEOR_ZPG_X ),
			std::make_pair( nitIMM, niocEOR_IMM ),
			std::make_pair( nitABS, niocEOR_ABS ),
			std::make_pair( nitABSX, niocEOR_ABS_X ),
			std::make_pair( nitABSY, niocEOR_ABS_Y )
		} ) ),

	std::make_pair( niiADC, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocADC_X_IND ),
			std::make_pair( nitIND_Y, niocADC_IND_Y ),
			std::make_pair( nitZPG, niocADC_ZPG ),
			std::make_pair( nitZPG_X, niocADC_ZPG_X ),
			std::make_pair( nitIMM, niocADC_IMM ),
			std::make_pair( nitABS, niocADC_ABS ),
			std::make_pair( nitABSX, niocADC_ABS_X ),
			std::make_pair( nitABSY, niocADC_ABS_Y )
		} ) ),

	std::make_pair( niiLDA, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocLDA_X_IND ),
			std::make_pair( nitIND_Y, niocLDA_IND_Y ),
			std::make_pair( nitZPG, niocLDA_ZPG ),
			std::make_pair( nitZPG_X, niocLDA_ZPG_X ),
			std::make_pair( nitIMM, niocLDA_IMM ),
			std::make_pair( nitABS, niocLDA_ABS ),
			std::make_pair( nitABSX, niocLDA_ABS_X ),
			std::make_pair( nitABSY, niocLDA_ABS_Y )
		} ) ),

	std::make_pair( niiCMP, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocCMP_X_IND ),
			std::make_pair( nitIND_Y, niocCMP_IND_Y ),
			std::make_pair( nitZPG, niocCMP_ZPG ),
			std::make_pair( nitZPG_X, niocCMP_ZPG_X ),
			std::make_pair( nitIMM, niocCMP_IMM ),
			std::make_pair( nitABS, niocCMP_ABS ),
			std::make_pair( nitABSX, niocCMP_ABS_X ),
			std::make_pair( nitABSY, niocCMP_ABS_Y )
		} ) ),

	std::make_pair( niiSBC, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocSBC_X_IND ),
			std::make_pair( nitIND_Y, niocSBC_IND_Y ),
			std::make_pair( nitZPG, niocSBC_ZPG ),
			std::make_pair( nitZPG_X, niocSBC_ZPG_X ),
			std::make_pair( nitIMM, niocSBC_IMM ),
			std::make_pair( nitABS, niocSBC_ABS ),
			std::make_pair( nitABSX, niocSBC_ABS_X ),
			std::make_pair( nitABSY, niocSBC_ABS_Y )
		} ) ),

	std::make_pair( niiASL, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitACC, niocASL_ACC ),
			std::make_pair( nitZPG, niocASL_ZPG ),
			std::make_pair( nitZPG_X, niocASL_ZPG_X ),
			std::make_pair( nitABS, niocASL_ABS ),
			std::make_pair( nitABSX, niocASL_ABS_X )
		} ) ),

	std::make_pair( niiROL, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitACC, niocROL_ACC ),
			std::make_pair( nitZPG, niocROL_ZPG ),
			std::make_pair( nitZPG_X, niocROL_ZPG_X ),
			std::make_pair( nitABS, niocROL_ABS ),
			std::make_pair( nitABSX, niocROL_ABS_X )
		})),

		std::make_pair( niiLSR, NES_INSTRUCTION_CODE_MAP(
			{
				std::make_pair( nitACC, niocLSR_ACC ),
				std::make_pair( nitZPG, niocLSR_ZPG ),
				std::make_pair( nitZPG_X, niocLSR_ZPG_X ),
				std::make_pair( nitABS, niocLSR_ABS ),
				std::make_pair( nitABSX, niocLSR_ABS_X )
			} ) ),

	std::make_pair( niiROR, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitACC, niocROR_ACC ),
			std::make_pair( nitZPG, niocROR_ZPG ),
			std::make_pair( nitZPG_X, niocROR_ZPG_X ),
			std::make_pair( nitABS, niocROR_ABS ),
			std::make_pair( nitABSX, niocROR_ABS_X )
		} ) ),

	std::make_pair( niiSTA, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitX_IND, niocSTA_X_IND ),
			std::make_pair( nitIND_Y, niocSTA_IND_Y ),
			std::make_pair( nitZPG, niocSTA_ZPG ),
			std::make_pair( nitZPG_X, niocSTA_ZPG_X ),
			std::make_pair( nitABS, niocSTA_ABS ),
			std::make_pair( nitABSX, niocSTA_ABS_X ),
			std::make_pair( nitABSY, niocSTA_ABS_Y )
		} ) ),

	std::make_pair( niiLDY, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitIMM, niocLDY_IMM ),
			std::make_pair( nitZPG, niocLDY_ZPG ),
			std::make_pair( nitZPG_X, niocLDY_ZPG_X ),
			std::make_pair( nitABS, niocLDY_ABS ),
			std::make_pair( nitABSX, niocLDY_ABS_X )
		} ) ),

	std::make_pair( niiLDX, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitIMM, niocLDX_IMM ),
			std::make_pair( nitZPG, niocLDX_ZPG ),
			std::make_pair( nitZPG_Y, niocLDX_ZPG_Y ),
			std::make_pair( nitABS, niocLDX_ABS ),
			std::make_pair( nitABSY, niocLDX_ABS_Y )
		} ) ),

	std::make_pair( niiSTY, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitZPG, niocSTY_ZPG ),
			std::make_pair( nitZPG_X, niocSTY_ZPG_X ),
			std::make_pair( nitABS, niocSTY_ABS )
		} ) ),

	std::make_pair( niiSTX, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitZPG, niocSTX_ZPG ),
			std::make_pair( nitZPG_Y, niocSTX_ZPG_Y ),
			std::make_pair( nitABS, niocSTX_ABS )
		} ) ),

	std::make_pair( niiCPX, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitIMM, niocCPX_IMM ),
			std::make_pair( nitZPG, niocCPX_ZPG ),
			std::make_pair( nitABS, niocCPX_ABS )
		} ) ),

	std::make_pair( niiCPY, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitIMM, niocCPY_IMM ),
			std::make_pair( nitZPG, niocCPY_ZPG ),
			std::make_pair( nitABS, niocCPY_ABS )
		} ) ),

	std::make_pair( niiDEC, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitZPG, niocDEC_ZPG ),
			std::make_pair( nitZPG_X, niocDEC_ZPG_X ),
			std::make_pair( nitABS, niocDEC_ABS ),
			std::make_pair( nitABSY, niocDEC_ABS_X )
		} ) ),


	std::make_pair( niiINC, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitZPG, niocINC_ZPG ),
			std::make_pair( nitZPG_X, niocINC_ZPG_X ),
			std::make_pair( nitABS, niocINC_ABS ),
			std::make_pair( nitABSY, niocINC_ABS_X )
		} ) ),

	std::make_pair( niiBIT, NES_INSTRUCTION_CODE_MAP(
		{
			std::make_pair( nitZPG, niocBIT_ZPG ),
			std::make_pair( nitABS, niocBIT_ABS )
		} ) )
};