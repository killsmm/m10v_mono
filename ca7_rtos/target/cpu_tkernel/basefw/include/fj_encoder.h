/**
 * @file		fj_encoder.h
 * @brief		Encoder API Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_ENCODER_H
#define _FJ_ENCODER_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_overview
@{

  -# @ref Video_record_specification
  -# @ref Audio_record_specification

  @section Video_record_specification Video record specification

  <table style="text-align:left;" border="1" cellpadding="1" cellspacing="1">
    <tr>
      <th style="background-color: rgb(204, 255, 255);">Item</th>
      <th style="background-color: rgb(204, 255, 255);">Possible spec in BaseFW. <br>for H264</th>
      <th style="background-color: rgb(204, 255, 255);">Possible spec in BaseFW. <br>for H265</th>
      <th style="background-color: rgb(204, 255, 255);">Changeable</th>
      <th style="background-color: rgb(204, 255, 255);">Remarks</th>
    </tr>
    <tr>
      <th valign=top>Stream Format</th>
      <td valign=top>ES  (Elementary Stream)<br>
                     PES (Packetized Elementary Stream)</td>
      <td valign=top>ES  (Elementary Stream)<br>
                     PES (Packetized Elementary Stream)</td>
      <td valign=top>@ref FJ_VideoEncoder_Config().</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>Image Format</th>
      <td valign=top>ALL YCC420 (Video Format)</td>
      <td valign=top>ALL YCC420 (Video Format)</td>
      <td valign=top>-</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>Multi Encoding</th>
      <td valign=top>2 stream<br>
                     Total MAX performance of all H.264 stream are up to 4096x2160 30p.</td>
// --- REMOVE_60_30FPS BEGIN ---
      <td valign=top>2 stream<br>
                     Total MAX performance of all H.265 stream are up to 4096x2160 60p.</td>
// --- REMOVE_60_30FPS END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
      <td valign=top>2 stream<br>
                     Total MAX performance of all H.265 stream are up to 4096x2160 60p.</td>
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
      <td valign=top>2 stream<br>
                     Total MAX performance of all H.265 stream are up to 4096x2160 30p.</td>
// --- REMOVE_ONLY_30FPS END ---
      <td valign=top>@ref FJ_VideoEncoder_Config().</td>
      <td valign=top>It can be set within the limits of SDRAM bandwidth and MemoryMap.</td>
    </tr>
    <tr>
      <th valign=top>Recording Size</th>
      <td valign=top>
                     4096x2304<br>
                     4096x2160<br>
                     4096x2048<br>
                     3840x2160<br>
                     2704x2028<br>
                     2704x1520<br>
                     1920x1440<br>
                     1920x1080<br>
                     1920x960<br>
                     1440x1080<br>
                     1440x720<br>
                     1280x960<br>
                     1280x720<br>
                     960x480<br>
                     848x480<br>
                     720x480<br>
                     640x480<br>
                     640x360<br>
                     432x240<br>
                     320x240
      </td>
      <td valign=top>
                     4096x2160<br>
                     3840x2160<br>
                     3840x1920<br>
                     3000x3000<br>
                     2704x2028<br>
                     2704x1520<br>
                     2560x1920<br>
                     1920x1440<br>
                     1920x1080<br>
                     1504x1504<br>
                     1440x1080<br>
                     1440x720<br>
                     1280x960<br>
                     1280x720<br>
                     848x480
      </td>
      <td valign=top>@ref FJ_VideoEncoder_Config().</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>THM File</th>
      <td valign=top colspan="1">Create<br>
                                 Not Create</td>
      <td valign=top colspan="1">Create<br>
                                 Not Create</td>
      <td valign=top>Users FW can to create a THM by calling the @ref FJ_SetDemosaicOutputControl(). <br>
                     Please refer Video Record (Start-Loop-Stop) Sequence.</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>Frame Rate</th>
      <td valign=top>
                    240fps <br>
                    120fps <br>
                     60fps <br>
                     50fps <br>
                     48fps <br>
                     30fps <br>
                     25fps <br>
                     24fps <br>
                     15fps <br>
                    239.76fps <br>
                    119.88fps <br>
                     59.94fps <br>
                     47.95fps <br>
                     29.97fps <br>
                     23.97fps <br>
                     14.98fps <br>
      <td valign=top>
                    240fps <br>
                    120fps <br>
                     60fps <br>
                     30fps <br>
                     15fps <br>
                    239.76fps <br>
                    119.88fps <br>
                     59.94fps <br>
                     29.97fps <br>
                     14.98fps <br>
      <td valign=top>@ref FJ_VideoEncoder_Config().</td>
      <td valign=top>Can not be confused with 59.94fps group and 60fps group.<br>
                     And allowed conbination @ref FrameRate_and_Size_List_H264 and @ref FrameRate_and_Size_List_H265 </td>
    </tr>
    <tr>
      <th valign=top>Rate Control</th>
      <td valign=top>Please appoint it with numerical value.<br>
                     MAX:100Mbps</td>
      <td valign=top>Please appoint it with numerical value.<br>
                     MAX:120Mbps</td>
      <td valign=top>@ref FJ_VideoEncoder_Config()</td>
      <td valign=top>-</td>
    </tr>
  </table>
  <br>

  @section FrameRate_and_Size_List_H264

  <table style="text-align:left;" border="1" cellpadding="2" cellspacing="2">
    <tr>
      <th style="background-color: rgb(255, 255, 255);">-</th>
      <th style="background-color: rgb(204, 255, 255);">4096x<br>2304</th>
      <th style="background-color: rgb(204, 255, 255);">4096x<br>2160</th>
      <th style="background-color: rgb(204, 255, 255);">4096x<br>2048</th>
      <th style="background-color: rgb(204, 255, 255);">3840x<br>2160</th>
      <th style="background-color: rgb(204, 255, 255);">2704x<br>2028</th>
      <th style="background-color: rgb(204, 255, 255);">2704x<br>1520</th>
      <th style="background-color: rgb(204, 255, 255);">1920x<br>1440</th>
      <th style="background-color: rgb(204, 255, 255);">1920x<br>1080</th>
      <th style="background-color: rgb(204, 255, 255);">1440x<br>1080</th>
      <th style="background-color: rgb(204, 255, 255);">1440x<br>720</th>
      <th style="background-color: rgb(204, 255, 255);">1280x<br>960</th>
      <th style="background-color: rgb(204, 255, 255);">1280x<br>720</th>
      <th style="background-color: rgb(204, 255, 255);">848x<br>480</th>
      <th style="background-color: rgb(204, 255, 255);">720x<br>480</th>
      <th style="background-color: rgb(204, 255, 255);">640x<br>480</th>
      <th style="background-color: rgb(204, 255, 255);">640x<br>360</th>
      <th style="background-color: rgb(204, 255, 255);">432x<br>240</th>
      <th style="background-color: rgb(204, 255, 255);">320x<br>240</th>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">240fps<br>
                                                        239.76fps</th>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">120fps<br>
                                                        119.88fps</th>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">60fps<br>
                                                        59.94fps</th>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">50fps<br>
                                                            -</th>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">48fps<br>
                                                        47.95fps</th>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">30fps<br>
                                                        29.97fps</th>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">25fps<br>
                                                            -</th>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">24fps<br>
                                                        23.97fps</th>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">15fps<br>
                                                        14.98fps</th>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
  </table>

  @section FrameRate_and_Size_List_H265

  <table style="text-align:left;" border="1" cellpadding="2" cellspacing="2">
    <tr>
      <th style="background-color: rgb(255, 255, 255);">-</th>
      <th style="background-color: rgb(204, 255, 255);">4096x<br>2160</th>
      <th style="background-color: rgb(204, 255, 255);">3840x<br>2160</th>
      <th style="background-color: rgb(204, 255, 255);">3000x<br>3000</th>
      <th style="background-color: rgb(204, 255, 255);">2704x<br>2028</th>
      <th style="background-color: rgb(204, 255, 255);">2704x<br>1520</th>
      <th style="background-color: rgb(204, 255, 255);">2560x<br>1920</th>
      <th style="background-color: rgb(204, 255, 255);">1920x<br>1440</th>
      <th style="background-color: rgb(204, 255, 255);">1920x<br>1080</th>
      <th style="background-color: rgb(204, 255, 255);">1504x<br>1504</th>
      <th style="background-color: rgb(204, 255, 255);">1440x<br>1080</th>
      <th style="background-color: rgb(204, 255, 255);">1440x<br>720</th>
      <th style="background-color: rgb(204, 255, 255);">1280x<br>960</th>
      <th style="background-color: rgb(204, 255, 255);">1280x<br>720</th>
      <th style="background-color: rgb(204, 255, 255);">848x<br>480</th>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">240fps<br>
                                                        239.76fps</th>
// --- REMOVE_ES1_HARDWARE BEGIN ---
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">x</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
// --- REMOVE_ES3_HARDWARE END ---
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">120fps<br>
                                                        119.88fps</th>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
// --- REMOVE_60_30FPS BEGIN ---
      <td align="center">o</td>
      <td align="center">x</td>
      <td align="center">o</td>
// --- REMOVE_60_30FPS END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
      <td align="center">o</td>
      <td align="center">x</td>
      <td align="center">o</td>
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
// --- REMOVE_ONLY_30FPS END ---
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">60fps<br>
                                                        59.94fps</th>
// --- REMOVE_60_30FPS BEGIN ---
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
// --- REMOVE_60_30FPS END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
      <td align="center">x</td>
// --- REMOVE_ONLY_30FPS END ---
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">30fps<br>
                                                        29.97fps</th>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
    <tr>
      <th style="background-color: rgb(255, 204, 255);">15fps<br>
                                                        14.98fps</th>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
      <td align="center">o</td>
    </tr>
  </table>


  @section Audio_record_specification Audio record specification

  <table style="text-align:left;" border="1" cellpadding="1" cellspacing="1">
    <tr>
      <th style="background-color: rgb(204, 255, 255);">Item</th>
      <th style="background-color: rgb(204, 255, 255);">Possible spec in BaseFW. <br>for AAC</th>
      <th style="background-color: rgb(204, 255, 255);">Changeable</th>
      <th style="background-color: rgb(204, 255, 255);">Remarks</th>
    </tr>
    <tr>
      <th valign=top>Stream Format</th>
      <td valign=top>ES  (Elementary Stream)<br>
                     PES (Packetized Elementary Stream)</td>
      <td valign=top>-</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>Sampling Rate</th>
      <td valign=top>8000Hz<br>
                     12000Hz<br>
                     16000Hz<br>
                     24000Hz<br>
                     32000Hz<br>
                     48000Hz</td>
      <td valign=top>@ref FJ_AudioEncoder_Config()</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>Bit Rate</th>
      <td valign=top>sampling rate 8000[Hz]/Mono:8000-48000[bps]<br>
                     sampling rate 8000[Hz]/Stereo:16000-96000[bps]<br>
                     sampling rate 12000[Hz]/Mono:8000-72000[bps]<br>
                     sampling rate 12000[Hz]/Stereo:16000-144000[bps]<br>
                     sampling rate 16000[Hz]/Mono:8000-96000[bps]<br>
                     sampling rate 16000[Hz]/Stereo:16000-192000[bps]<br>
                     sampling rate 24000[Hz]/Mono:8000-144000[bps]<br>
                     sampling rate 24000[Hz]/Stereo:16000-256000[bps]<br>
                     sampling rate 32000[Hz]/Mono:8000-192000[bps]<br>
                     sampling rate 32000[Hz]/Stereo:16000-256000[bps]<br>
                     sampling rate 48000[Hz]/Mono:8000-256000[bps]<br>
                     sampling rate 48000[Hz]/Stereo:16000-256000[bps]</td>
      <td valign=top>FJ_AudioEncoder_Config()</td>
      <td valign=top>-</td>
    </tr>
    <tr>
      <th valign=top>Channel(Stereo/Mono)</th>
      <td valign=top>Stereo, Mono</td>
      <td valign=top>@ref FJ_AudioEncoder_Config()</td>
      <td valign=top>-</td>
    </tr>
  </table>
  <br>

@}*//* --- end of fj_encoder_overview */


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_sequence
@{
	- Video Record
	-# @ref fj_movie_record_seq_1
	-# @ref fj_movie_record_seq_2
	-# @ref fj_movie_record_seq_3

	<hr>
	@section fj_movie_record_seq_1 Video Record (Start-Loop-Stop)
	@image html fj_movie_record.png

	@section fj_movie_record_seq_2 Video Record (VideoTimeLapse)
	@image html fj_movie_record_VideoTimeLapse.png

	@section fj_movie_record_seq_3 Video Record Error
	@image html fj_movie_record_error.png

@}*//* --- end of fj_encoder_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_definition
@{*/
/* ON/OFF flag */
#define FJ_MOVIE_OFF					0	/**< flag OFF. */
#define FJ_MOVIE_ON						1	/**< flag ON. */

/* Codec type */
#define FJ_MOVIE_CODEC_H264				0	/**< H264 Codec. */
#define FJ_MOVIE_CODEC_H265				1	/**< H265 Codec. */
#define FJ_MOVIE_CODEC_AAC				2	/**< AAC Coded. */

/* Configure Type */
#define FJ_MOVIE_H264_CFG				0	/**< H264 Codec configration. */
#define FJ_MOVIE_H265_CFG				1	/**< H265 Codec configration. */

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_STREAM_FORMAT_ES			0	/**< StreamFormat : "ES"  (Elementary Stream) */
#define FJ_MOVIE_STREAM_FORMAT_PES			1	/**< StreamFormat : "PES" (Packetized Elementary Stream) */
#define FJ_MOVIE_STREAM_FORMAT_ES_HEADER	2	/**< StreamFormat : "ES"  (Elementary Stream but Header attached every first frame of GOP) */

/* For wOperation in FJ_Video_EncoderConfigure() */
#define FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR			0		/**< CBR. (Constant Bit Rate) */
#define FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR			1		/**< VBR. (Variable Bit Rate)*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CVBR			2		/**< CVBR. (Capped Variable Bit Rate)*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_ABR			2		/**< ABR. (Average Bit Rate)*/
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
#define FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_FIXED_QP		2

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP	0	/**< GOP structure type : "IPPP.." */
#define FJ_MOVIE_VIDEO_GOP_STRUCT_IBP	1	/**< GOP structure type : "IBP.." */
#define FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP	2	/**< GOP structure type : "IBBP.." */
#define FJ_MOVIE_VIDEO_GOP_STRUCT_IIII	3	/**< GOP structure type : "IIII.." (It support only when GOP num is 1 or 2.)*/

/* Frame Type */
#define FJ_MOVIE_VIDEO_FRAME_TYPE_IDR	0	/**< Frame Type is IDR */
#define FJ_MOVIE_VIDEO_FRAME_TYPE_I		1	/**< Frame Type is I */
#define FJ_MOVIE_VIDEO_FRAME_TYPE_P		2	/**< Frame Type is P */
#define FJ_MOVIE_VIDEO_FRAME_TYPE_B		3	/**< Frame Type is B */

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_CLOSED_GOP_ON		0	/**< Close GOP */
#define FJ_MOVIE_VIDEO_CLOSED_GOP_OFF		1	/**< Open GOP */

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_PROFILE_HIGH					0	/**< PROFILE : "High" */
#define FJ_MOVIE_VIDEO_PROFILE_MAIN					1	/**< PROFILE : "Main" */
#define FJ_MOVIE_VIDEO_PROFILE_CONSTRAINED_BASELINE	2	/**< PROFILE : "Constrained Baseline" */

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_LEVEL_1				10	/**< LEVEL : "1" */
#define FJ_MOVIE_VIDEO_LEVEL_1_3			13	/**< LEVEL : "1.3" */
#define FJ_MOVIE_VIDEO_LEVEL_2				20	/**< LEVEL : "2" */
#define FJ_MOVIE_VIDEO_LEVEL_2_1			21	/**< LEVEL : "2.1" */
#define FJ_MOVIE_VIDEO_LEVEL_2_2			22	/**< LEVEL : "2.2" */
#define FJ_MOVIE_VIDEO_LEVEL_3				30	/**< LEVEL : "3" */
#define FJ_MOVIE_VIDEO_LEVEL_3_1			31	/**< LEVEL : "3.1" */
#define FJ_MOVIE_VIDEO_LEVEL_3_2			32	/**< LEVEL : "3.2" */
#define FJ_MOVIE_VIDEO_LEVEL_4				40	/**< LEVEL : "4" */
#define FJ_MOVIE_VIDEO_LEVEL_4_1			41	/**< LEVEL : "4.1" */
#define FJ_MOVIE_VIDEO_LEVEL_4_2			42	/**< LEVEL : "4.2" */
#define FJ_MOVIE_VIDEO_LEVEL_5				50	/**< LEVEL : "5" */
#define FJ_MOVIE_VIDEO_LEVEL_5_1			51	/**< LEVEL : "5.1" */
#define FJ_MOVIE_VIDEO_LEVEL_5_2			52	/**< LEVEL : "5.2" */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define FJ_MOVIE_VIDEO_LEVEL_6				60	/**< LEVEL : "6" */
#define FJ_MOVIE_VIDEO_LEVEL_6_1			61	/**< LEVEL : "6.1" */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_VLC_MODE_CABAC		0	/**< VLCMODE : "CABAC" */
#define FJ_MOVIE_VIDEO_VLC_MODE_CAVLC		1	/**< VLCMODE : "CAVLC" */

/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_SCAN_MODE_PROGRESSIVE	0				/**< progressive. */
#define FJ_MOVIE_VIDEO_SCAN_MODE_INTERLACE		1				/**< interlace. (Not Supported)*/

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_VIDEO_TIER_MAIN			0	/**< Main Tier */
#define FJ_MOVIE_VIDEO_TIER_HIGH			1	/**< High Tier */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

///////////////////////////////////////////////////////////////////////////////
// enumeration
///////////////////////////////////////////////////////////////////////////////
/** Video Encode Enumelation */
typedef enum {
	E_FJ_VIDEO_ENC_ENUM_H264_0 = 0,		/**< H264_0 */
	E_FJ_VIDEO_ENC_ENUM_H264_1 = 1,		/**< H264_1 */
	E_FJ_VIDEO_ENC_ENUM_H264_2 = 2,		/**< H264_2 */
	E_FJ_VIDEO_ENC_ENUM_H265_0 = 10,	/**< H265_0 */
	E_FJ_VIDEO_ENC_ENUM_H265_1 = 11,	/**< H265_1 */
	E_FJ_VIDEO_ENC_ENUM_MAX			/**< Stopper */
} E_FJ_VIDEO_ENC_ENUM;

/** Still Encode Enumelation */
typedef enum {
	E_FJ_STILL_ENC_ENUM_JPEG_0 = 20,	/**< JPEG_0 */
	E_FJ_STILL_ENC_ENUM_JPEG_1 = 21,	/**< JPEG_1 */
	E_FJ_STILL_ENC_ENUM_JPEG_2 = 22,	/**< JPEG_2 */
	E_FJ_STILL_ENC_ENUM_JPEG_3 = 23,	/**< JPEG_3 */
	E_FJ_STILL_ENC_ENUM_JPEG_4 = 24,	/**< JPEG_4 */
	E_FJ_STILL_ENC_ENUM_MAX			/**< Stopper */
} E_FJ_STILL_ENC_ENUM;

/** Image Size */
typedef enum {
	E_FJ_MOVIE_VIDEO_SIZE_4000_3000	= 0,	/**< 4000x3000 */
	E_FJ_MOVIE_VIDEO_SIZE_4096_2304	= 1,	/**< 4096x2304 */
	E_FJ_MOVIE_VIDEO_SIZE_4096_2160	= 2,	/**< 4096x2160 */
	E_FJ_MOVIE_VIDEO_SIZE_4096_2048	= 3,	/**< 4096x2048 */
	E_FJ_MOVIE_VIDEO_SIZE_3840_2160	= 4,	/**< 3840x2160 */
	E_FJ_MOVIE_VIDEO_SIZE_3840_1920	= 5,	/**< 3840x1920 */
	E_FJ_MOVIE_VIDEO_SIZE_3000_3000	= 6,	/**< 3000x3000 */
	E_FJ_MOVIE_VIDEO_SIZE_2704_2028	= 7,	/**< 2704x2028 */
	E_FJ_MOVIE_VIDEO_SIZE_2704_1520	= 8,	/**< 2704x1520 */
	E_FJ_MOVIE_VIDEO_SIZE_2560_1920	= 9,	/**< 2560x1920 */
	E_FJ_MOVIE_VIDEO_SIZE_1920_1440	= 10,	/**< 1920x1440 */
	E_FJ_MOVIE_VIDEO_SIZE_1920_1080	= 11,	/**< 1920x1080 */
	E_FJ_MOVIE_VIDEO_SIZE_1920_960	= 12,	/**< 1920x960 */
	E_FJ_MOVIE_VIDEO_SIZE_1504_1504	= 13,	/**< 1504x1504 */
	E_FJ_MOVIE_VIDEO_SIZE_1440_1080	= 14,	/**< 1440x1080 */
	E_FJ_MOVIE_VIDEO_SIZE_1440_720	= 15,	/**< 1440x 720 */
	E_FJ_MOVIE_VIDEO_SIZE_1280_960	= 16,	/**< 1280x 960 */
	E_FJ_MOVIE_VIDEO_SIZE_1280_720	= 17,	/**< 1280x 720 */
	E_FJ_MOVIE_VIDEO_SIZE_960_480	= 18,	/**<  960x 480 */
	E_FJ_MOVIE_VIDEO_SIZE_864_480	= 19,	/**<  864x 480 */
	E_FJ_MOVIE_VIDEO_SIZE_848_480	= 20,	/**<  848x 480 */
	E_FJ_MOVIE_VIDEO_SIZE_720_480	= 21,	/**<  720x 480 */
	E_FJ_MOVIE_VIDEO_SIZE_640_480	= 22,	/**<  640x 480 */
	E_FJ_MOVIE_VIDEO_SIZE_640_360	= 23,	/**<  640x 360 */
	E_FJ_MOVIE_VIDEO_SIZE_432_240	= 24,	/**<  432x 240 */
	E_FJ_MOVIE_VIDEO_SIZE_320_240	= 25,	/**<  320x 240 */
	E_FJ_MOVIE_VIDEO_SIZE_720_576	= 26,
	E_FJ_MOVIE_VIDEO_SIZE_1024_768	= 27,
	E_FJ_MOVIE_VIDEO_SIZE_1280_768	= 28,
	E_FJ_MOVIE_VIDEO_SIZE_1280_1024	= 29,
	E_FJ_MOVIE_VIDEO_SIZE_800_600	= 30,
	E_FJ_MOVIE_VIDEO_SIZE_2560_1440	= 31,	/**< 2560x1440 */
	E_FJ_MOVIE_VIDEO_SIZE_960_540	= 32,
	E_FJ_MOVIE_VIDEO_SIZE_480_270	= 33,
	E_FJ_MOVIE_VIDEO_SIZE_1600_896	= 34,
	E_FJ_MOVIE_VIDEO_SIZE_1024_576	= 35,
	E_FJ_MOVIE_VIDEO_SIZE_800_448	= 36,
	E_FJ_MOVIE_VIDEO_SIZE_352_288	= 37,
	E_FJ_MOVIE_VIDEO_SIZE_2592_1944	= 38,
	E_FJ_MOVIE_VIDEO_SIZE_2304_1296	= 39,
	E_FJ_MOVIE_VIDEO_SIZE_1920_1280 = 40,
	E_FJ_MOVIE_VIDEO_SIZE_640_512 = 41,
	E_FJ_MOVIE_VIDEO_SIZE_384_288 = 42,
	E_FJ_MOVIE_VIDEO_SIZE_5472_3648 = 43,
	E_FJ_MOVIE_VIDEO_SIZE_6144_4096 = 44,
	E_FJ_MOVIE_VIDEO_SIZE_6244_4168 = 45,
} E_FJ_MOVIE_VIDEO_SIZE;


/* AudioEncoder */
/** Audio Encode Type */
typedef enum {
  E_FJ_AUDIO_ENC_NONE_0 = 0x00, 	/**< Non-Encoding. */
  E_FJ_AUDIO_ENC_AAC_0,				/**< AAC-LC 0. */
  E_FJ_AUDIO_ENC_AAC_1,				/**< AAC-LC 1. */
} E_FJ_AUDIO_ENC_ENUM;

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/** VUI Information (Please check the H.264 standard for more information.) */
typedef struct {
	UCHAR				video_signal_type_present_flag;		/**< OFF(0)/ON(1) */
	UCHAR				video_format;						/**< 3bit */
	UCHAR				video_full_range_flag;				/**< OFF(0)/ON(1) */
	UCHAR				colour_description_present_flag;	/**< OFF(0)/ON(1) */
	UCHAR				colour_primaries;					/**< 8bit */
	UCHAR				transfer_characteristics;			/**< 8bit */
	UCHAR				matrix_coefficients;				/**< 8bit */
	UCHAR				aspect_ratio_info_present_flag;		/**< OFF(0)/ON(1) */
	UCHAR				aspect_ratio_idc;					/**< 8bit */
	USHORT				sar_width;							/**< 16bit */
	USHORT				sar_height;							/**< 16bit */
	UCHAR				chroma_loc_info_present_flag;		/**< OFF(0)/ON(1) */
	UCHAR				chroma_sample_loc_type_top_field;	/**< 0~6 */
	UCHAR				chroma_sample_loc_type_bottom_field;/**< 0~6 */
} T_FJ_MOVIE_H264_VUI_PARAM;

/** ScalingList Initial Information (Please check the H.264 standard for more information.)*/
typedef struct {
	INT32				scaling_list_flg;			/**< Flag to Enable ScalingList */
	INT32				scaling_list_4x4_Y1[16];	/**< ScalingList 4x4 for Y1 */
	INT32				scaling_list_4x4_Cb1[16];	/**< ScalingList 4x4 for Cb1 */
	INT32				scaling_list_4x4_Cr1[16];	/**< ScalingList 4x4 for Cr1 */
	INT32				scaling_list_4x4_Y2[16];	/**< ScalingList 4x4 for Y2 */
	INT32				scaling_list_4x4_Cb2[16];	/**< ScalingList 4x4 for Cb2 */
	INT32				scaling_list_4x4_Cr2[16];	/**< ScalingList 4x4 for Cr2 */
	INT32				scaling_list_8x8_Y1[64];	/**< ScalingList 8x8 for Y1 */
	INT32				scaling_list_8x8_Y2[64];	/**< ScalingList 8x8 for Y2 */
} T_FJ_MOVIE_H264_INITIAL_SCALINGLIST;

/** H.264 encoder config parameter */
typedef struct {
	UINT8						enable_flag;			/**< Encoder Enable Flag. (FJ_MOVIE_OFF or FJ_MOVIE_ON) */
	E_FJ_MOVIE_VIDEO_SIZE		image_size;				/**< Select of Image Size.<br><br>
															 E_FJ_MOVIE_VIDEO_SIZE_4000_3000  : 0   (4000x3000) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_4096_2304  : 1   (4096x2304) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_4096_2160  : 2   (4096x2160) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_4096_2048  : 3   (4096x2048) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_3840_2160  : 4   (3840x2160) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_2704_2028  : 6   (2704x2028) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_2704_1520  : 7   (2704x1520) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1920_1440  : 9   (1920x1440) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1920_1080  : 10  (1920x1080) <br>
															 E_FJ_MOVIE_VIDEO_SIZE_1920_960  : 11  (1920x960) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1440_1080  : 12  (1440x1080) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1440_720   : 13  (1440x1080) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1280_960   : 14  (1280x 960) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1280_720   : 15  (1280x 720) <br>
															 E_FJ_MOVIE_VIDEO_SIZE_960_480    : 16  ( 960x 480) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_864_480    : 17  ( 864x 480) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_848_480    : 18  ( 848x 480) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_720_480    : 19  ( 720x 480) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_640_480    : 20  ( 640x 480) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_640_360    : 21  ( 640x 360) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_432_240    : 22  ( 432x 240) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_320_240    : 23  ( 320x 240) <br> */
	UINT8						stream_format;			/**< Select of stream format.<br><br>
															 FJ_MOVIE_STREAM_FORMAT_ES  : 0 (Elementary Stream)<br>
															 FJ_MOVIE_STREAM_FORMAT_PES : 1 (Packetized Elementary Stream)<br>
															 FJ_MOVIE_STREAM_FORMAT_ES_HEADER : 2 (Elementary Stream but Header attached every first frame of GOP)<br><br>
															 In the case of "ES", SPS/PPS will be granted only to the first Stream. <br>
															 In the case of "PES", SPS/PPS will be granted every IDR.
															 In the case of "ES_HEADER", SPS/PPS will be granted every IDR. */
	UINT8						bitrate_algorithm;		/**< Bitrate Algorithm.<br><br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR : 0 (Constant Bit Rate)<br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR : 1 (Variable Bit Rate)<br> */
	UINT32						bitrate;				/**< Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 Case of CBR, this parameter means "target bitrate".<br>
															 Case of VBR, this parameter means "MAX bitrate". <br> */
	UINT32						average_bitrate;		/**< Average Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 Case of CBR, this parameter invalid.<br>
															 Case of VBR, this parameter means "average bitrate". <br> */
	UINT32						frame_rate;				/**< Set Value of Frame Rate.<br>
															 (ex. 29.97fps -> D'2997, 30fps -> D'3000)<br>
															 Can set the value, refer to @ref Video_record_specification.<br> */
	UINT8						gop_struct;				/**< Stream GOP Struct.<br><br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IBP  : 1 (B-frame Distance = 1)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 2 (B-frame Distance = 2)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IIII : 3 (I-frame Only) (It support only when frame_num_of_gop is 1 or 2.)<br> */
	UINT8						frame_num_of_gop;		/**< Set Velue of the number of frames of GOP.<br><br>
															 (ex. 1GOP is 15frame => set 15.)<br>
															 The range of possible values is the Frame number of common divisors of FrameRate. <br>
															 In the case of IBBP structure, it must be a multiple of 3. <br>
															 However, if it is set to 44 or more, "REW x1 play" (ReversePlay) is impossible. */
	UINT8						closed_gop_flag;		/**< Select of Closed GOP or Open GOP. <br><br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_ON  : 0(Closed Gop)<br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_OFF : 1(Open Gop)<br> */
	UINT8						gop_num_idr_to_idr;		/**< Set Value of GOP number for IDR to IDR.<br><br>
															 (ex. [IDR,P,,,P][IDR,P,,,P][IDR,P,,,P],,,  is set value is 1.)<br>
															 (ex. [IDR,P,,,P][I,P,,,P][IDR,P,,,P],,,  is set value is 2.)<br>
															 When OpenGOP, it's valid.<br> */
	UINT8						profile;				/**< Select of Profile of H.264 standard. <br><br>
															 FJ_MOVIE_VIDEO_PROFILE_HIGH                  : 0(High)<br>
															 FJ_MOVIE_VIDEO_PROFILE_MAIN                  : 1(Main)<br>
															 FJ_MOVIE_VIDEO_PROFILE_CONSTRAINED_BASELINE  : 2(Constrained Baseline)<br> */
	UINT8						level;					/**< Select of Level of H.264 standard.<br><br>
															 FJ_MOVIE_VIDEO_LEVEL_1_3   : 13  (level 1.3)<br>
															 FJ_MOVIE_VIDEO_LEVEL_2     : 20 (level 2)<br>
															 FJ_MOVIE_VIDEO_LEVEL_2_1   : 21 (level 2.1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_2_2   : 22 (level 2.2)<br>
															 FJ_MOVIE_VIDEO_LEVEL_3     : 30 (level 3)<br>
															 FJ_MOVIE_VIDEO_LEVEL_3_1   : 31 (level 3,1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_3_2   : 32 (level 3.2)<br>
															 FJ_MOVIE_VIDEO_LEVEL_4     : 40 (level 4)<br>
															 FJ_MOVIE_VIDEO_LEVEL_4_1   : 41 (level 4.1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_4_2   : 42 (level 4.2)<br>
															 FJ_MOVIE_VIDEO_LEVEL_5     : 50 (level 5)<br>
															 FJ_MOVIE_VIDEO_LEVEL_5_1   : 51 (level 5.1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_5_2   : 52 (level 5.2)<br> */
	UINT8						entropy_coding;			/**< Select of Entropy coding patarn of h.264 standard.<br><br>
															 FJ_MOVIE_VIDEO_VLC_MODE_CABAC : 0(CABAC)<br>
															 FJ_MOVIE_VIDEO_VLC_MODE_CAVLC : 1(CAVLC)<br> */
	UINT8						buffer_ctrl_flag;		/**< Set Flag to Insert hrd_parameters in VUI/BuffeingPeriodSEI/removal_delay in PictureTimingSEI. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						pictstruct_flag;		/**< Set Flag to Pictstruct in PictureTiming SEI. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 When buffer_ctrl_flag is on, it's valid.<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						recovery_point_sei_flag;/**< Set Flag to Insert Recovery Point SEI. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						end_of_stream_flag;		/**< Set Flag to Insert End of Stream. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						end_of_sequence_flag;	/**< Set Flag to Insert End of Sequence. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						filler_flag;			/**< Set Flag to Insert Filler Data. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						vector_info_flag;		/**< Set Flag to Notify the Vector/Brightness Information. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 If this flag is ON, FJ_Host_H264VectorCB() is valid.<br> */
	UINT8						time_lapse_flag;		/**< Set Flag to TimeLapse Mode. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 If this flag is ON, Encoder is not automatically encodeing.<br> */
	T_FJ_MOVIE_H264_VUI_PARAM	vui_param;				/**< Set VUI Information in SPS of H.264 stream.<br>
															 Please check the H.264 standard for more information.<br> */
	T_FJ_MOVIE_H264_INITIAL_SCALINGLIST	scaling_list;	/**< Set SalingList Information in SPS  of H.264 stream.<br>
															 Please check the H.264 standard for more information.<br> */
	UINT8						slice_encode_flag;		/**< Slice Encode Mode Enable Flag. (FJ_MOVIE_OFF or FJ_MOVIE_ON)
															 FJ_MOVIE_ON  : Codec doing slice divide encodeing. Latency of encode will be less, but the image quality will low.
															 FJ_MOVIE_OFF : Codec doing frame encodeing. <br><br>
															 *Attention* This Parameter is invalid if Image Line is 768 or less. And This Parameter is invalid if stream_format is ES. */
// --- REMOVE_H264_IQ_ENABLE BEGIN ---
	UINT8						prefetch_bypath_flag;	/**< */
// --- REMOVE_H264_IQ_ENABLE END ---
	
	UCHAR						delay_notify_flag;		/**< delay notify to host for stable interval. <br> */
	UINT16						extend_gop_num;
} T_FJ_MOVIE_H264_CONFIG_PARAM;

/** H.264 encoder dynamic config parameter */
typedef struct {
	UINT32						bitrate;				/**< Dynamic Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 This parameter is valid at the CBR.<br>
															 This parameter means "target bitrate".<br> */
	UINT8						gop_struct;				/**< Stream GOP Struct.<br><br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IBP  : 1 (B-frame Distance = 1)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 2 (B-frame Distance = 2)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IIII : 3 (I-frame Only) (It support only when frame_num_of_gop is 1 or 2.)<br> */
	UINT16						frame_num_of_gop;		/**< Set Velue of the number of frames of GOP.<br><br>
															 (ex. 1GOP is 15frame => set 15.)<br>
															 The range of possible values is the Frame number of common divisors of FrameRate. <br>
															 In the case of IBBP structure, it must be a multiple of 3. <br>
															 However, if it is set to 44 or more, "REW x1 play" (ReversePlay) is impossible. */
	UINT8						closed_gop_flag;		/**< Select of Closed GOP or Open GOP. <br><br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_ON  : 0(Closed Gop)<br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_OFF : 1(Open Gop)<br><br>
															 This parameter is only valid at the timing of the GOP including the IDR. <br> */
	UINT8						gop_num_idr_to_idr;		/**< Set Value of GOP number for IDR to IDR.<br><br>
															 (ex. [IDR,P,,,P][IDR,P,,,P][IDR,P,,,P],,,  is set value is 1.)<br>
															 (ex. [IDR,P,,,P][I,P,,,P][IDR,P,,,P],,,  is set value is 2.)<br><br>
															 When OpenGOP, it's valid.<br>
															 This parameter is only valid at the timing of the GOP including the IDR<br>  */
} T_FJ_MOVIE_H264_DYNAMIC_CONFIG_PARAM;

/** VUI Information (Please check the H.265 standard for more information.) */
typedef struct {
	UCHAR				video_signal_type_present_flag;		/**< OFF(0)/ON(1) */
	UCHAR				video_format;						/**< 3bit */
	UCHAR				video_full_range_flag;				/**< OFF(0)/ON(1) */
	UCHAR				colour_description_present_flag;	/**< OFF(0)/ON(1) */
	UCHAR				colour_primaries;					/**< 8bit */
	UCHAR				transfer_characteristics;			/**< 8bit */
	UCHAR				matrix_coefficients;				/**< 8bit */
	UCHAR				aspect_ratio_info_present_flag;		/**< 8bit */
	UCHAR				aspect_ratio_idc;					/**< 8bit */
	USHORT				sar_width;							/**< 16bit */
	USHORT				sar_height;							/**< 16bit */
	UCHAR				chroma_loc_info_present_flag;		/**< OFF(0)/ON(1) */
	UCHAR				chroma_sample_loc_type_top_field;	/**< 0~6 */
	UCHAR				chroma_sample_loc_type_bottom_field;/**< 0~6 */
} T_FJ_MOVIE_H265_VUI_PARAM;

/** H.265 encoder config parameter */
typedef struct {
	UINT8						enable_flag;			/**< Encoder Enable Flag. (FJ_MOVIE_OFF or FJ_MOVIE_ON) */
	E_FJ_MOVIE_VIDEO_SIZE		image_size;				/**< Select of Image Size.<br><br>
														     E_FJ_MOVIE_VIDEO_SIZE_4000_3000  : 0   (4000x3000) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_4096_2160  : 2   (4096x2160) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_3840_2160  : 4   (3840x2160) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_3000_3000  : 5   (3000x3000) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_2704_2028  : 6   (2704x2028) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_2704_1520  : 7   (2704x1520) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_2560_1920  : 8   (2560x1920) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1920_1440  : 9   (1920x1440) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1920_1080  : 10  (1920x1080) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1504_1504  : 11  (1504x1504) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1440_1080  : 12  (1440x1080) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1440_720   : 13  (1440x 720) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1280_960   : 14  (1280x 960) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_1280_720   : 15  (1280x 720) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_864_480    : 16  ( 864x 480) <br>
														     E_FJ_MOVIE_VIDEO_SIZE_848_480    : 17  ( 848x 480) <br> */
	UINT8						stream_format;			/**< Select of stream format.<br><br>
															 FJ_MOVIE_STREAM_FORMAT_ES  : 0 (Elementary Stream)<br>
															 FJ_MOVIE_STREAM_FORMAT_PES : 1 (Packetized Elementary Stream)<br>
															 FJ_MOVIE_STREAM_FORMAT_ES_HEADER : 2 (Elementary Stream but Header attached every first frame of GOP)<br><br>
															 In the case of "ES", VPS/SPS/PPS will be granted only to the first Stream. <br>
															 In the case of "PES", VPS/SPS/PPS will be granted every IDR.
															 In the case of "ES_HEADER", VPS/SPS/PPS will be granted every IDR. */
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT8						bitrate_algorithm;		/**< Bitrate Algorithm.<br><br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR  : 0 (Constant Bit Rate)<br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR  : 1 (Variable Bit Rate)<br> 
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CVBR : 2 (Capped Variable Bit Rate)<br> */
	UINT32						bitrate;				/**< Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 Case of CBR, this parameter means "target bitrate".<br>
															 Case of VBR, this parameter means "MAX bitrate". <br> */
	UINT32						average_bitrate;		/**< Average Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 Case of CBR, this parameter invalid.<br>
															 Case of VBR, this parameter means "average bitrate". <br> */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT8						bitrate_algorithm;		/**< Bitrate Algorithm.<br><br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR  : 0 (Constant Bit Rate)<br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR  : 1 (Variable Bit Rate)<br>
															 FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_ABR  : 2 (Average Bit Rate)<br>*/
	UINT32						bitrate;				/**< Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 Case of CBR, this parameter means "target bitrate".<br>
															 Case of VBR, this parameter means "MAX bitrate". <br>
															 Case of ABR, this parameter is always 0. <br>*/
	UINT32						average_bitrate;		/**< Average Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 Case of CBR, please set same as "bitrate".<br>
															 Case of VBR/ABR, this parameter means "average bitrate". <br> */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	UINT32						frame_rate;				/**< Set Value of Frame Rate.<br>
															 (ex. 29.97fps -> D'2997, 30fps -> D'3000)<br>
															 Can set the value, refer to @ref Video_record_specification.<br> */
	UINT8						gop_struct;				/**< Stream GOP Struct.<br><br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 2 (B-frame Distance = 2)<br> */
	UINT8						frame_num_of_gop;		/**< Set Velue of the number of frames of GOP.<br><br>
															 (ex. 1GOP is 15frame => set 15.)<br>
															 The range of possible values is the Frame number of 0.25 second or 0.5 second or 1 second of FrameRate.
															 In the case of IBBP structure, it must be a multiple of 3. */
	UINT8						closed_gop_flag;		/**< Select of Closed GOP or Open GOP. <br><br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_ON  : 0(Closed Gop)<br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_OFF : 1(Open Gop)<br> */
	UINT8						gop_num_idr_to_idr;		/**< Set Value of GOP number for IDR to IDR.<br><br>
															 (ex. [IDR,P,,,P][IDR,P,,,P][IDR,P,,,P],,,  is set value is 1.)<br>
															 (ex. [IDR,P,,,P][I,P,,,P][IDR,P,,,P],,,  is set value is 2.)<br>
															 When OpenGOP, it's valid.<br> */
	UINT8						level;					/**< Select of Level of H.265 standard.<br><br>
															 FJ_MOVIE_VIDEO_LEVEL_1     : 10 (level 1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_2     : 20 (level 2)<br>
															 FJ_MOVIE_VIDEO_LEVEL_2_1   : 21 (level 2.1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_3     : 30 (level 3)<br>
															 FJ_MOVIE_VIDEO_LEVEL_3_1   : 31 (level 3,1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_4     : 40 (level 4)<br>
															 FJ_MOVIE_VIDEO_LEVEL_4_1   : 41 (level 4.1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_5     : 50 (level 5)<br>
															 FJ_MOVIE_VIDEO_LEVEL_5_1   : 51 (level 5.1)<br>
															 FJ_MOVIE_VIDEO_LEVEL_5_2   : 52 (level 5.2)<br>
															 FJ_MOVIE_VIDEO_LEVEL_6     : 60 (level 6)<br>
															 FJ_MOVIE_VIDEO_LEVEL_6_1   : 61 (level 6.1)<br> */
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT8						tier;					/**< Select of Tier of H.265 standard.<br><br>
															 FJ_MOVIE_VIDEO_TIER_MAIN   : 0 (Main Tier)<br>
															 FJ_MOVIE_VIDEO_TIER_HIGH   : 1 (High Tier)<br> */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	UINT8						buffer_ctrl_flag;		/**< Set Flag to Insert hrd_parameters in VUI/BuffeingPeriodSEI/removal_delay in PictureTimingSEI. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.265 standard for more information.<br> */
	UINT8						pictstruct_flag;		/**< Set Flag to Pictstruct in PictureTiming SEI. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 When buffer_ctrl_flag is on, it's valid.<br>
															 Please check the H.265 standard for more information.<br> */
	UINT8						filler_flag;			/**< Set Flag to Insert Filler Data. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 Please check the H.265 standard for more information.<br> */
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT8						vector_info_flag;		/**< Set Flag to Notify the Vector/Brightness Information. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 If this flag is ON, FJ_Host_H265VectorCB() is valid.<br> */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	UINT8						time_lapse_flag;		/**< Set Flag to TimeLapse Mode. (FJ_MOVIE_OFF or FJ_MOVIE_ON)<br>
															 If this flag is ON, Encoder is not automatically encodeing.<br> */
	T_FJ_MOVIE_H265_VUI_PARAM	vui_param;				/**< Set VUI Information in SPS of H.265 stream.<br>
															 Please check the H.265 standard for more information.<br> */
	UINT8						slice_encode_flag;		/**< Slice Encode Mode Enable Flag. (FJ_MOVIE_OFF or FJ_MOVIE_ON) <br>
															 FJ_MOVIE_ON  : Codec doing slice divide encodeing. Latency of encode will be less, but the image quality will low. <br>
															 FJ_MOVIE_OFF : Codec doing frame encodeing. <br>
															 This Parameter is invalid if stream_format is ES. <br>*/
	UINT8						oneshot_encode_flag;	/**< 1Shot Encode Mode Enable Flag. (FJ_MOVIE_OFF or FJ_MOVIE_ON) <br>
															 FJ_MOVIE_ON  : Codec doing 1shot encodeing. In this mode Codec will autonomously stop. <br>
															 FJ_MOVIE_OFF : Codec doing normal encodeing. <br> */
	UCHAR						delay_notify_flag;		/**< delay notify to host for stable interval. <br>*/
	UINT32						slice_int_line;
} T_FJ_MOVIE_H265_CONFIG_PARAM;

/** H.265 encoder dynamic config parameter */
typedef struct {
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT32						bitrate;				/**< Dynamic Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 This parameter is valid at the CVBR.<br>
															 This parameter means "average bitrate". *Please set under than MAX bitrate when set at start.<br> */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT32						bitrate;				/**< Dynamic Bitrate. <br>
															 Please Set Value of Bitrate.<br>
															 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
															 This parameter means "average bitrate". *Please set under than MAX bitrate when set at start.<br> */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	UINT8						gop_struct;				/**< Stream GOP Struct.<br><br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
															 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 2 (B-frame Distance = 2)<br> */
	UINT8						frame_num_of_gop;		/**< Set Velue of the number of frames of GOP.<br><br>
															 (ex. 1GOP is 15frame => set 15.)<br>
															 The range of possible values is the Frame number of 0.25 second or 0.5 second or 1 second of FrameRate.
															 In the case of IBBP structure, it must be a multiple of 3. */
	UINT8						closed_gop_flag;		/**< Select of Closed GOP or Open GOP. <br><br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_ON  : 0(Closed Gop)<br>
															 FJ_MOVIE_VIDEO_CLOSED_GOP_OFF : 1(Open Gop)<br><br>
															 This parameter is only valid at the timing of the GOP including the IDR. <br> */
	UINT8						gop_num_idr_to_idr;		/**< Set Value of GOP number for IDR to IDR.<br><br>
															 (ex. [IDR,P,,,P][IDR,P,,,P][IDR,P,,,P],,,  is set value is 1.)<br>
															 (ex. [IDR,P,,,P][I,P,,,P][IDR,P,,,P],,,  is set value is 2.)<br><br>
															 When OpenGOP, it's valid.<br>
															 This parameter is only valid at the timing of the GOP including the IDR<br>  */
} T_FJ_MOVIE_H265_DYNAMIC_CONFIG_PARAM;

/** H264 Vector information parameter */
typedef struct{
	ULLONG		frame_no;				/**< Frame Number to be notified continuously form View.<br>
											 This parameter will start counting from getting to VideoMode. */
	UINT32		width;					/**< Stream Encoding Width Size */
	UINT32		lines;					/**< Stream Encoding Lines Size */
	UINT8		frame_type;				/**< Stream Frame Type.<br><br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_IDR : 0 (IDR Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_I   : 1 (I Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_P   : 2 (P Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_B   : 3 (B Frame) */
	UINT8		gop_struct;				/**< Stream GOP Struct.<br><br>
											 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
											 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 1 (B-frame Distance = 2)<br>
											 FJ_MOVIE_VIDEO_GOP_STRUCT_IIII : 2 (I-frame Only)<br> */
	UINT8		frame_num_of_gop;		/**< The number of frames of GOP.<br> */
	LLONG		stream_pts;				/**< Stream PTS.<br>
											 This PTS is based on 90KHz timescale. */
	LLONG		stream_dts;				/**< Stream DTS.<br>
											 This DTS is based on 90KHz timescale. */
	UINT32		bitrate;				/**< Notify current Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000) <br>
											 Case of CBR, this parameter means "target bitrate".<br>
											 Case of VBR, this parameter means "MAX bitrate". <br> */
	UINT32		ave_bitrate;			/**< Notify current Average Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000)<br>
											 Case of CBR, this parameter invalid.<br>
											 Case of VBR, this parameter means "average bitrate". <br> */
	UINT32		vector_addr;			/**< Notify top address of Vector Data of first macro block.<br>
											 Please refer FJ_Host_H264VectorCB() for struct of 1macro block data. */
	UINT32		vector_size;			/**< Notify total size of Vector Data of total macro block.<br>
											 Please refer FJ_Host_H264VectorCB() for struct of 1macro block data. */
	UINT32		brightness_addr;		/**< Notify top address of Brightness Data of first macro block.<br>
											 Please refer FJ_Host_H264VectorCB() for struct of 1macro block data. */
	UINT32		brightness_size;		/**< Notify total size of Brightness Data of total macro block.<br>
											 Please refer FJ_Host_H264VectorCB() for struct of 1macro block data. */
} T_FJ_MOVIE_H264_VECTOR_PARAM;

/** H.264 Video stream parameter */
typedef struct{
	ULLONG		frame_no;				/**< Frame Number to be notified continuously form View.<br>
											 This parameter will start counting from getting to VideoMode. */
	UINT32		width;					/**< Stream Encoding Width Size */
	UINT32		lines;					/**< Stream Encoding Lines Size */
	ULLONG		stream_serial_num;		/**< Stream Serial Number.<br>
											 It will increase by 1 for each of the notification. */
	BOOL		stream_end_flg;			/**< Each Stream End Flag.<br>
											 It becomes TRUE at the time of the last Frame notification. */
	UINT32		vstream_index;			/**< Video stream index.<br>
											 Please use this parameter at stream free request in FJ_VideoEncoder_FreeIndex(). */
	UINT32		stream_address;			/**< Stream address */
	UINT32		stream_size;			/**< Stream size */
	UINT8		frame_type;				/**< Stream Frame Type.<br><br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_IDR : 0 (IDR Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_I   : 1 (I Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_P   : 2 (P Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_B   : 3 (B Frame) */
	LONG		stream_poc;				/**< Stream Picture Order Count */
	UINT32		counter_in_gop;			/**< Current frame counter in GOP.<br>
											 When it comes to the next GOP will be reset. */
	UINT32		VBV_buffer;				/**< Current VBV Buffer Value.<br>
											 Please check the H.264 standard for more information.<br> */
	UINT32		nal_unit_num;			/**< NUL unit number in current stream. */
	ULONG		nal_unit_size[11];		/**< Each NAL unit size in current stream.<br>
											 Valid array component will match the value of "nal_unit_num". */
	LLONG		stream_pts;				/**< Stream PTS.<br>
											 This PTS is based on 90KHz timescale. */
	LLONG		stream_dts;				/**< Stream DTS.<br>
											 This DTS is based on 90KHz timescale. */
	UINT32		bitrate;				/**< Notify current Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000)<br>
											 Case of CBR, this parameter means "target bitrate".<br>
											 Case of VBR, this parameter means "MAX bitrate". <br> */
	UINT32		ave_bitrate;			/**< Notify current Average Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000)<br>
											 Case of CBR, this parameter invalid.<br>
											 Case of VBR, this parameter means "average bitrate". <br> */
	BOOL		error_flag;				/**< Notify error flag.<br>
											 Normal case is FALSE. If codec error occurred, this flag is TRUE.<br>
											 If this flag is TRUE, other parameters are invalid. */
} T_FJ_MOVIE_H264_STREAM_PARAM;

// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/** H265 Vector information parameter */
typedef struct{
	ULLONG		frame_no;				/**< Frame Number to be notified continuously form View.<br>
											 This parameter will start counting from getting to VideoMode. */
	UINT32		width;					/**< Stream Encoding Width Size */
	UINT32		lines;					/**< Stream Encoding Lines Size */
	UINT8		frame_type;				/**< Stream Frame Type.<br><br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_IDR : 0 (IDR Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_I   : 1 (I Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_P   : 2 (P Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_B   : 3 (B Frame) */
	UINT8		gop_struct;				/**< Stream GOP Struct.<br><br>
											 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
											 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 1 (B-frame Distance = 2)<br>
											 FJ_MOVIE_VIDEO_GOP_STRUCT_IIII : 2 (I-frame Only)<br> */
	UINT8		frame_num_of_gop;		/**< The number of frames of GOP.<br>*/
	LLONG		stream_pts;				/**< Stream PTS.<br>
											 This PTS is based on 90KHz timescale. */
	LLONG		stream_dts;				/**< Stream DTS.<br>
											 This DTS is based on 90KHz timescale. */
	UINT32		bitrate;				/**< Notify current Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000) <br>
											 Case of CBR, this parameter means "target bitrate".<br>
											 Case of VBR, this parameter means "MAX bitrate". <br> */
	UINT32		ave_bitrate;			/**< Notify current Average Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000)<br>
											 Case of CBR, this parameter invalid.<br>
											 Case of VBR, this parameter means "average bitrate". <br> */
	UINT32		vector_addr;			/**< Notify top address of Vector Data. */
	UINT32		vector_size;			/**< Notify total size of Vector Data. */
} T_FJ_MOVIE_H265_VECTOR_PARAM;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---

/** H.265 Video stream parameter */
typedef struct{
	ULLONG		frame_no;				/**< Frame Number to be notified continuously form View.<br>
											 This parameter will start counting from getting to VideoMode. */
	UINT32		width;					/**< Stream Encoding Width Size */
	UINT32		lines;					/**< Stream Encoding Lines Size */
	ULLONG		stream_serial_num;		/**< Stream Serial Number.<br>
											 It will increase by 1 for each of the notification. */
	UCHAR		stream_end_flg;			/**< Each Stream End Flag.<br> */
	UINT32		vstream_index;			/**< Video stream index.<br>
											 Please use this parameter at stream free request in FJ_VideoEncoder_FreeIndex(). */
	UINT32		stream_address;			/**< Stream address */
	UINT32		stream_size;			/**< Stream size */
	UINT8		frame_type;				/**< Stream Frame Type.<br><br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_IDR : 0 (IDR Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_I   : 1 (I Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_P   : 2 (P Frame)<br>
											 FJ_MOVIE_VIDEO_FRAME_TYPE_B   : 3 (B Frame) */
	LONG		stream_poc;				/**< Stream Picture Order Count */
	UINT32		counter_in_gop;			/**< Current frame counter in GOP.<br>
											 When it comes to the next GOP will be reset. */
	UINT32		nal_unit_num;			/**< NUL unit number in current stream. */
	ULONG		nal_unit_size[32];		/**< Each NAL unit size in current stream.<br>
											 Valid array component will match the value of "nal_unit_num". */
	LLONG		stream_pts;				/**< Stream PTS.<br>
											 This PTS is based on 90KHz timescale. */
	LLONG		stream_dts;				/**< Stream DTS.<br>
											 This DTS is based on 90KHz timescale. */
	UINT32		bitrate;				/**< Notify current Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000)<br>
											 Case of CBR, this parameter means "target bitrate".<br>
											 Case of VBR, this parameter means "MAX bitrate". <br> */
	UINT32		ave_bitrate;			/**< Notify current Average Bitrate. <br>
											 (ex. 24Mbps -> D'24,000,000)<br>
											 Case of CBR, this parameter invalid.<br>
											 Case of VBR, this parameter means "average bitrate". <br> */
	BOOL		error_flag;				/**< Notify error flag.<br>
											 Normal case is FALSE. If codec error occurred, this flag is TRUE.<br>
											 If this flag is TRUE, other parameters are invalid. */
} T_FJ_MOVIE_H265_STREAM_PARAM;

/* AudioEncoder */
/** AudioEncoder config parameter(AAC) */
typedef struct {
	UINT8	enable_flag;		/**< AudioEncoder Enable Flag. (FJ_AUDIO_ENABLE or FJ_AUDIO_DISABLE) */
	ULONG	sampling_rate;		/**< Sampling rate */
	ULONG	bit_rate;			/**< Bit rate */
	UCHAR	out_ch_type;		/**< Channel type(Mono is FJ_AUDIO_NUM_CHANNEL_1. Stereo is FJ_AUDIO_NUM_CHANNEL_2.)*/
} T_FJ_AUDIO_AAC_CONFIG_PARAM;


/** AAC stream callback informations */
typedef struct {
	UCHAR		area_index;				/**< The index of stream buffer area */
	UCHAR		num_remain_area;		/**< The number of available remainder stream buffer area */
	UCHAR*		uac_data;				/**< Audio Packetized UAC Pointer */
	ULONG		uac_size;				/**< Audio Packetized UAC Size */
	UCHAR*		pes_data;				/**< Audio Packetized Elementary Stream Pointer */
	ULONG		pes_size;				/**< Audio Packetized Elementary Stream Size */
	UCHAR*		adts_data;				/**< Audio data transport stream Pointer */
	ULONG		adts_size;				/**< Audio data transport stream Size */
	UCHAR*		es_data;				/**< Audio Elementary Stream Pointer */
	ULONG		es_size;				/**< Audio Elementary Stream Size */
	UINT64		time_stamp;				/**< Audio Time Stamp<br>
											 This Time Stamp is based on 90kHz timescale. */
	ULONG		error_info;				/**< Error Information */
	ULONG		ch_type;				/**< Channel type of audio stream data[1:Mono, 2:Stereo] */
	ULONG		frame_length;			/**< The number of samples included in the frame */
	ULONG		sampling_freq;			/**< Sampling Frequency */
	ULONG		bit_rate;				/**< Bit Rate */
	UCHAR		stream_end_flag;		/**< Flag Audio is stopped */
	ULLONG		frame_no;
} T_FJ_AUDIO_AAC_STREAM_PARAM;

/** PCM stream callback informations */
typedef struct {
	UCHAR		area_index;				/**< The index of stream buffer area */
	UCHAR		num_remain_area;		/**< The number of available remainder stream buffer area */
	UCHAR*		pcm_data;				/**< Audio PCM Pointer */
	ULONG		pcm_size;				/**< Audio PCM Size */
	UCHAR*		uac_data;				/**< Audio PCM Pointer */
	ULONG		uac_size;				/**< Audio PCM Size */
	UINT64		time_stamp;				/**< Audio Time Stamp<br>
											 This Time Stamp is based on 90kHz timescale. */
	ULONG		error_info;				/**< Error Information */
	ULONG		ch_type;				/**< Channel type of audio stream data[1:Mono, 2:Stereo] */
	ULONG		frame_length;			/**< The number of samples included in the frame */
	ULONG		sampling_freq;			/**< Sampling Frequency */
	ULONG		bit_rate;				/**< Bit Rate */
	UCHAR		stream_end_flag;		/**< Flag Audio is stopped */
	ULLONG		frame_no;
} T_FJ_AUDIO_PCM_STREAM_PARAM;

/*@} --- end of fj_encoder_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API opens the Encode HW (get instance).

@param[in]	video_enc			Encode H/W type. see @ref E_FJ_VIDEO_ENC_ENUM.
@param[out]	video_enc_id		Encode instance ID.<br>
								Based on this ID, it will identify the Stream.
@retval FJ_ERR_OK : success.
@retval FJ_ERR_NG : fail.
 */
extern	FJ_ERR_CODE FJ_VideoEncoder_Open( E_FJ_VIDEO_ENC_ENUM video_enc, UCHAR* video_enc_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API closes the Encode HW (release instance).

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval FJ_ERR_OK : success.
@retval FJ_ERR_NG : fail.
 */
extern	FJ_ERR_CODE FJ_VideoEncoder_Close( UCHAR video_enc_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API sets whether video encode instance is enabled.<br><br>

This API will be used in FJ_HostVideoYUVManipulation() if VideoTimeLapse is enabled.<br>
If the instance is enabled, it will encode the Frame of the target.<br>

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param[in]	enabled			TRUE:  Encode enable<br>
							FALSE: Encode disable
@retval		FJ_ERR_OK : success
@retval		FJ_ERR_NG : fail - FJ error code according to the specific error.
*/
extern	FJ_ERR_CODE	FJ_VideoEncoder_SetInstanceEnabled( UCHAR video_enc_id, BOOL enabled );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure the parameters of the codec at the before start.

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param[in] wConfig			Set Configure Type.<br>
							FJ_MOVIE_H264_CFG or FJ_MOVIE_H265_CFG.
@param[in] wOperation		Please set of start address of configure structure.<br>
							T_FJ_MOVIE_H264_CONFIG_PARAM or T_FJ_MOVIE_H265_CONFIG_PARAM.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
@remark This API must have been Call before FJ_VideoEncoder_Setup().
*/
extern	FJ_ERR_CODE	FJ_VideoEncoder_Config( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure the parameters of the codec at the during operation.

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param[in] wConfig			Set Configure Type.<br>
							FJ_MOVIE_H264_CFG or FJ_MOVIE_H265_CFG.
@param[in] wOperation		Please set of start address of configure structure.<br>
							T_FJ_MOVIE_H264_DYNAMIC_CONFIG_PARAM or T_FJ_MOVIE_H265_DYNAMIC_CONFIG_PARAM.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
@remark This function, please call in the FJ_Host_EncoderSequenceStartCB()
*/
extern	FJ_ERR_CODE	FJ_VideoEncoder_ConfigDynamic( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the trriger of inserting the IDR Picture.

@param[in] video_enc_id		Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param[in] wConfig			Set Configure Type.<br>
							FJ_MOVIE_H264_CFG or FJ_MOVIE_H265_CFG.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
@remark This function, please call during the video mode.
*/
FJ_ERR_CODE FJ_VideoEncoder_InsertIDR( UCHAR video_enc_id, UINT32 wConfig );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function setup the codec and parameter check.

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
@remarks Please call this API before Video Codec Start by all means.
*/

extern	FJ_ERR_CODE	FJ_VideoEncoder_Setup( UCHAR video_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start the encoder.

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
*/

extern	FJ_ERR_CODE	FJ_VideoEncoder_Start( UCHAR video_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stop the encoder.

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
*/

extern	FJ_ERR_CODE	FJ_VideoEncoder_Stop( UCHAR video_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Free H.264/H.265 video stream.

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param [in] free_vstream_index	Free H.264/H.265 stream index.<br>
								This Index is compatible with the Index to be notified in the FJ_Host_H264StreamCB() or FJ_Host_H265StreamCB().
@param [in] last_free_flg		Please return the FALSE during normal  sequence.<br>
								Please return the TRUE during the final Stream release.<br>
								BaseFW wait for the end processing until this flag is TRUE.
@retval FJ_ERR_OK	: Success
@retval FJ_ERR_NG	: Error occurred.
*/
extern	FJ_ERR_CODE FJ_VideoEncoder_FreeIndex( UCHAR video_enc_id, UINT32 free_vstream_index, BOOL last_free_flg );

extern FJ_ERR_CODE FJ_VideoEncoder_CompEnable( UCHAR video_enc_id, BOOL comp_enable );


/* Still Encoder */
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API opens the Encode HW (get instance).

@param[in]	still_enc			Encode H/W type. see @ref E_FJ_STILL_ENC_ENUM.
@param[out]	still_enc_id		Encode instance ID.<br>
@retval FJ_ERR_OK : success.
@retval FJ_ERR_NG : fail.
 */
extern	FJ_ERR_CODE FJ_StillEncoder_Open( E_FJ_STILL_ENC_ENUM still_enc, UCHAR* still_enc_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API closes the Encode HW (release instance).

@param[in]	still_enc_id	Encode instance ID.<br>
@retval FJ_ERR_OK : success.
@retval FJ_ERR_NG : fail.
 */
extern	FJ_ERR_CODE FJ_StillEncoder_Close( UCHAR still_enc_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API sets whether still encode instance is enabled.<br><br>

This API will be used in FJ_HostStillYUVManipulation() if JPEG encode is enabled.<br>
If the instance is enabled, it will encode the Frame of the target.<br>

@param[in]	still_enc_id	Encode instance ID.<br>
@param[in]	enabled			TRUE:  Encode enable<br>
							FALSE: Encode disable
@retval		FJ_ERR_OK : success
@retval		FJ_ERR_NG : fail - FJ error code according to the specific error.
*/
extern	FJ_ERR_CODE	FJ_StillEncoder_SetInstanceEnabled( UCHAR still_enc_id, BOOL enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API frees JPEG buffers(Main/Screennail/Thumbnail) of BaseFW.
 *	@param[in]	still_enc_id	: Still encode ID.
 *	@param[in]	jpg_idx			: Jpeg index(included in jpg_info of FJ_HostStillJpgManipulation()).
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(still_enc_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_FreeJpegBuffer( UCHAR still_enc_id, UINT32 jpg_idx );

/* AudioEncoder */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function open Audio Encoder instance.<br>
@param[in]	audio_enc			Audio Encoder.<br>
								[E_FJ_AUDIO_ENC_NONE_0, E_FJ_AUDIO_ENC_AAC_0, E_FJ_AUDIO_ENC_AAC_1]<br>
@param[out]	audio_enc_id	Audio Encoder ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
*/
extern FJ_ERR_CODE FJ_AudioEncoder_Open( E_FJ_AUDIO_ENC_ENUM audio_enc,  UCHAR * audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close Audio Encode instance.<br>
@param[in]	audio_enc_id	Audio Encoder ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_AudioEncoder_Close( UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Free Audio stream.<BR>

@param[in]	audio_enc_id		Audio Encoder ID.<br>
@param[in] free_astream_index	Free AAC stream index.<br>
								This Index is compatible with the Index to be notified in the FJ_Host_AACStreamCB().
@param[in] last_free_flg		Please return the FALSE during normal  sequence.<br>
								Please return the TRUE during the final Stream release.<br>
								BaseFW wait for the end processing until this flag is TRUE.
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioEncoder_FreeIndex( UCHAR audio_enc_id, UINT32 free_astream_index, BOOL last_free_flg );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure the audio encoder parameter.

@param[in]	audio_enc_id	Audio Encoder ID.<br>
@param[in] wConfig			Set Config Type.[FJ_AUDIO_AAC_CFG]<br>
@param[in] wOperation		Please set of start address of configure structure(T_FJ_AUDIO_AAC_CONFIG_PARAM).<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks Please call this API before Audio Codec Setup(FJ_Audio_EncoderSetup).<br>
If the corresponding compression format has become added, add the "Configure Type" and "configure structure".<br><br>
Sample code is as follows.<br>
@code
extern FJ_ERR_CODE error_code;
T_FJ_AUDIO_AAC_CONFIG_PARAM aac_config;

aac_config.enable_flag = FJ_AUDIO_ENABLE;
aac_config.sampling_rate = 48000;
aac_config.bit_rate = 192000;
aac_config.out_ch_type = FJ_AUDIO_NUM_CHANNEL_2;

error_code = FJ_AudioEncoder_Config(audio_enc_id, FJ_AUDIO_AAC_CFG, (UINT32 *)&aac_config);
@endcode
*/
extern	FJ_ERR_CODE FJ_AudioEncoder_Config( UCHAR audio_enc_id, UINT32 wConfig, UINT32 * wOperation );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function setup the AudioEncoder.<br>
It adopts the parameters set by FJ_Audio_EncoderConfig().
@param[in]	audio_enc_id	Audio Encoder ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks Please call this function before FJ_Audio_EncoderStart().
*/
extern	FJ_ERR_CODE	FJ_AudioEncoder_Setup( UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start the AudioEncoder.
@param[in]	audio_enc_id	Audio Encoder ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@retval		FJ_ERR_NG_TMOUT Time Out error.
@remarks	Please call this function after having started AudioCapture.<br>
			If AudioCapture is not started, this function waits.<br>
			(Wait time is time for 1024 samples.)<br>
*/
extern	FJ_ERR_CODE	FJ_AudioEncoder_Start( UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stop the AudioEncoder.
@param[in]	audio_enc_id	Audio Encoder ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern	FJ_ERR_CODE	FJ_AudioEncoder_Stop( UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function switches the sound data to be encoded.<br>
The encoded sound data are changed from the data of AudioCapture to the silence data.<br>
(The data of the AudioCapture -> The silence data)<br>
@param[in]	audio_enc_id	Audio Encoder ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern	FJ_ERR_CODE	FJ_AudioEncoder_StartMute( UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function switches the sound data to be encoded.<br>
The encoded sound data are changed from the silence data to the data of AudioCapture.<br>
(The silence data -> The data of the AudioCapture)<br>
@param[in]	audio_enc_id	Audio Encoder ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks	Please call this function after FJ_AudioEncoder_StartMute().<br>
*/
extern	FJ_ERR_CODE	FJ_AudioEncoder_StopMute( UCHAR audio_enc_id );


/*@} --- end of fj_encoder_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_message
@{*/
/**
@def FJ_HM_MOVIE_H264_CODEC_ERROR
This message means that there are any issues in the h264 codec. (It is necessary to reset of codec.)<br>
*/


/*@} --- end of fj_encoder_message */

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_callback
@{*/
/*@} --- end of fj_encoder_callback */

#endif
