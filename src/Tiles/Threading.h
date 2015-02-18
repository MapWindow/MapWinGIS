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
 // lsu 17 apr 2012 - created the file

#pragma once

// Represents task for asynchronous execution
class ITask
{
public:
	virtual void DoTask() = 0;
};

// A thread worker to run asyncronous task compatible with ATL CThreadPool  
class ThreadWorker
{
public:
    typedef DWORD_PTR RequestType;
	ThreadWorker() : m_dwExecs( 0 ) {}

    virtual BOOL Initialize(void *pvParam) { return TRUE;}
	virtual void Terminate(void* ) {}
	virtual void Execute(RequestType dw, void *pvParam, OVERLAPPED* pOverlapped) throw()
	{
		ITask* pTask = (ITask*)(DWORD_PTR)dw;
		pTask->DoTask();
		m_dwExecs++;
	}
    virtual BOOL GetWorkerData(DWORD, void **) { return FALSE; }
protected:
	DWORD	m_dwExecs;
}; 
