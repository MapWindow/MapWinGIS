/**************************************************************************************
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
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
 #pragma once
#include "ITileLoader.h"

// ******************************************************
//    TileBulkLoader()
// ******************************************************
// Handles the loading queue of map tiles, schedules the loaded tiles for caching
class TileBulkLoader: public ITileLoader
{
public:
	TileBulkLoader(ITileCache* cache)
		: _cache(cache)
	{
		_errorCount = 0;
		_sumCount = 0;
	}

	virtual ~TileBulkLoader(void)
	{
	}

private:
	// caching only
	ITileCache* _cache;
	ICallback* _callback;			 // to report progress to clients via COM interface
	IStopExecution* _stopCallback;	 // to stop execution by clients via COM interface
	int _errorCount;
	int _sumCount;                   // sums all requests even if generation doesn't match

private:
	void CleanTasks();

public:
	// properties
	void set_Callback(ICallback* callback) { _callback = callback; }
	void set_StopCallback(IStopExecution* callback) { _stopCallback = callback; }
	int get_ErrorCount() { return _errorCount; }
	int get_SumCount() { return _sumCount; }
	ITileCache* get_Cache() { return _cache; }

public:
	//methods
	void TileLoaded(TileCore* tile, int generation);
	void ResetErrorCount() { _errorCount = 0; _sumCount = 0; }
	ILoadingTask* CreateTask(int x, int y, int zoom, BaseProvider* provider, int generation);
};

