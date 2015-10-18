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
#include "TileBulkLoader.h"

class PrefetchManager;

// ******************************************************
//    PrefetchManagerFactory
// ******************************************************
class PrefetchManagerFactory
{
private:
	static vector<PrefetchManager*> _managers;
	static ::CCriticalSection _lock;
public:
	static PrefetchManager* Create(ITileCache* cache);
	static void Clear();
};

// ******************************************************
//    PrefetchManager
// ******************************************************
class PrefetchManager
{
public:
	PrefetchManager(ITileCache* cache)
		: _loader(cache)
	{
			
	}

private:
	TileBulkLoader _loader;

private:
	// methods
	void BuildDownloadList(BaseProvider* provider, int zoom, CRect indices, vector<TilePoint*>& points);
public: 
	// properties
	TileBulkLoader* get_Loader() { return &_loader; }

public:
	// methods
	long Prefetch(BaseProvider* provider, CRect indices, int zoom, ICallback* callback, IStopExecution* stop);
	void LogBulkDownloadStarted(int zoom);
	
};
