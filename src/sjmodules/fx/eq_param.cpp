/*******************************************************************************
 *
 *                                 Silverjuke
 *     Copyright (C) 2016 Björn Petersen Software Design and Development
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 *******************************************************************************
 *
 * File:    eq_param.cpp
 * Authors: Björn Petersen
 * Purpose: Handle equalizer parameters, usable without the equalizer itself
 *
 ******************************************************************************/


#include <sjbase/base.h>


const char* SjEqParam::s_bandNames[SJ_EQ_BANDS] =
	{ "55", "77", "110", "156", "220", "311", "440", "622", "880", "1.2", "1.8", "2.5", "3.5", "5", "7", "10", "14", "20" };
	// to save horizontal space, we use "Hz" for the lower bands and "KHz" starting with "1.2"; in the dialog we show a legend for this


bool SjEqParam::IsEqualTo(const SjEqParam& o) const
{
	for(int i=0; i<SJ_EQ_BANDS; i++) {
		if(m_bandDb[i]!=o.m_bandDb[i]) {
			return false; // at least on band is different
		}
	}

	return true; // all bands are equal
}


wxString SjEqParam::ToString(const wxString& sep) const
{
	wxString ret;
	for( int i = 0; i < SJ_EQ_BANDS; i++ )
	{
		ret += wxString::Format("%.1f"+sep, m_bandDb[i]);
	}

	ret.Replace(",", ".");      // force using the point instead of the comma as decimal separator
	ret.Replace(".0"+sep, sep); // shorten values with a single zero after the comma
	return ret;
}


void SjEqParam::FromString(const wxString& str__)
{
	wxString str(str__);
	str.Replace(" ", "");
	str.Replace("\r", "");
	str.Replace("\n", ";"); // the bands may be separated by a semicolon (used in the INI) for by a new-line (used in *.feq-files)

	wxArrayString bands = SjTools::Explode(str, ';', SJ_EQ_BANDS, SJ_EQ_BANDS);
	for( int i = 0; i < SJ_EQ_BANDS; i++ )
	{
		m_bandDb[i] = SjTools::ParseFloat(bands[i], 0.0F);
		if( m_bandDb[i] < SJ_EQ_BAND_MIN ) { m_bandDb[i] = SJ_EQ_BAND_MIN; }
		if( m_bandDb[i] > SJ_EQ_BAND_MAX ) { m_bandDb[i] = SJ_EQ_BAND_MAX; }
	}
}


void SjEqParam::Shift(float add)
{
	for( int i = 0; i < SJ_EQ_BANDS; i++ )
	{
		m_bandDb[i] += add;
		if( m_bandDb[i] < SJ_EQ_BAND_MIN ) { m_bandDb[i] = SJ_EQ_BAND_MIN; }
		if( m_bandDb[i] > SJ_EQ_BAND_MAX ) { m_bandDb[i] = SJ_EQ_BAND_MAX; }
	}
}


float SjEqParam::GetAutoLevelShift()
{
	// Get offset that moves all bands below 0.0 - avoids overdrive and distortions.
	// Moreover, if possible, move all bands up - to be as loud as possible.
	float maxGainAbove0 = 0.0F, minGainBelow0 = 666.6F;
	for( int i = 0; i < SJ_EQ_BANDS; i++ )
	{
		if( m_bandDb[i] > maxGainAbove0 ) {
			maxGainAbove0 = m_bandDb[i];
		}
		else if( m_bandDb[i] < 0.0F ) {
			if( (m_bandDb[i]*-1) < minGainBelow0 ) {
				minGainBelow0 = (m_bandDb[i]*-1);
			}
		}
	}

	if( maxGainAbove0 > 0.0F ) {
		return maxGainAbove0 * -1;
	}
	else if( minGainBelow0 < 666.6F ) {
		for( int i = 0; i < SJ_EQ_BANDS; i++ ) {
			if( m_bandDb[i]+minGainBelow0 > 0.0F ) { return 0.0F; }
		}
		return minGainBelow0;
	}

    return 0.0F;
}

