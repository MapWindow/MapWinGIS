/**************************************************************************************
 * File name: PointSymbols.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 // lsu 27 feb 2011 - Created the file.

#pragma once

void get_DefaultSymbolOptions(tkDefaultPointSymbol symbol, tkPointShapeType& type, double& rotation, int& numSides, float& sidesRatio);
float* get_SimplePointShape(tkPointShapeType type, float& size, double& rotation, int& numSides, float& sidesRatio, int* numPoints);
float* get_RegularShape(float sizeX, float sizeY, int numSides, double rotation);
float* get_StarShape(float sizeX, float sizeY, int numSides, float shapeRatio, double rotation);
float* get_CrossShape(float size, int numSides, float shapeRatio, double rotation);
float* get_ArrowShape(float sizeX, float sizeY, double rotation);
float* get_FlagShape(float sizeX, float sizeY, double rotation);
float* get_SimplePointShape(tkDefaultPointSymbol symbol, float size, int* numPoints);

