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
#include "stdafx.h"
#include "TileBulkLoader.h"
#include "BulkLoadingTask.h"

// *******************************************************
//		CreateTask()
// *******************************************************
ILoadingTask* TileBulkLoader::CreateTask(int x, int y, int zoom, BaseProvider* provider, int generation)
{
	return new BulkLoadingTask(x, y, zoom, provider, generation);
}

// *******************************************************
//		TileLoaded()
// *******************************************************
void TileBulkLoader::TileLoaded(TileCore* tile, int generation)
{
	_sumCount++;

	if (tile->hasErrors())
	{
		_errorCount++;
	}

	if (_callback != NULL)
	{
		TileRequestInfo* info = FindRequest(generation);
		if (info)
		{
			CallbackHelper::Progress(_callback, info->count >= info->totalCount ? -1 : info->count, "Caching...");
		}
	}

	if (_stopCallback)
	{
		VARIANT_BOOL stop;
		_stopCallback->StopFunction(&stop);

		if (stop && !_stopped)
		{
			_stopped = true;
			CallbackHelper::Progress(_callback, -2, "Caching...");
		}
	}

	if (!tile->IsEmpty())
	{
		_cache->AddTile(tile);
	}
}