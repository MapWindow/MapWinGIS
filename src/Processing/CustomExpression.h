/**************************************************************************************
 * File name: Expression.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CExpression
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
 
#pragma once
#include "ExpressionParts.h"

class CustomExpression
{
private:
	
public:
	CustomExpression() 
		: _useFields(true), _saveOperations(true), _floatFormat(m_globalSettings.floatNumberFormat),
		_shape(NULL), _errorPosition(-1)
	{
		
	}

	~CustomExpression()
	{
		Clear();
	}

private:
	std::vector<CExpressionPart*> _parts;
	std::vector<CElement*> _variables;
	std::vector<CString> _fields;
	std::vector<COperation*> _operations;
	std::vector<CString> _strings;
	bool _useFields;
	bool _saveOperations;
	CString _errorMessage;	// the description of error
	int _errorPosition;		// the position of error
	CString _floatFormat;
	IShape* _shape;

private:
	
	// calculation of expression
	bool FindOperation(CExpressionPart* part, COperation& operation); 
	bool CalculateOperation(CExpressionPart* part,  COperation& operation); 
	inline CExpressionValue* GetValue(CExpressionPart* part, int elementId);

	bool EvaluateFunction(CExpressionPart* part);
	void EvaluatePart();
	bool EvaluatePart(CExpressionPart* part, CString& errorMessage, int& operationCount);
	bool FinishPart(CExpressionPart* part);

	bool CalculateNextOperationWithinPart(CExpressionPart* part, CString& errorMessage, int& operationCount);

	void SetFieldValues(ITable* tbl);
	void BuildFieldList();
	void ResetActiveCountForParts();
	void Reset();

	TwoArgOperator GetMatrixOperation(tkOperation op);

public:
	bool IsEmpty() { return _parts.size() == 0; }
	bool Parse(CString s, bool useFields, CString& error);
	CExpressionValue* Calculate(CString& errorMessage);
	void Clear();
	void ReleaseArrays();

	bool ReadFieldNames(ITable* tbl);
	void SetFields(vector<CString>& fields);
	void AddPart(CExpressionPart* part) { _parts.push_back(part); }

	CString GetFloatFormat() { return _floatFormat; }
	void SetFloatFormat(CString value) { _floatFormat = value; }
	bool GetUseFields() { return _useFields; }
	
	vector<CString>* GetStrings() { return &_strings; }
	vector<CString>* GetFields() { return &_fields; }

	// variable fields
	int get_NumFields() { return _variables.size(); }
	int get_FieldIndex(int FieldId)	{ return _variables[FieldId]->fieldIndex; }
	CString get_FieldName(int FieldId) { return _variables[FieldId]->fieldName; }
	CExpressionValue* get_FieldValue(int FieldId) {	return _variables[FieldId]->val;}
	void put_FieldValue(int FieldId, double newVal) { _variables[FieldId]->val->dbl(newVal); }
	void put_FieldValue(int FieldId, BSTR newVal);
	void put_FieldValue(int FieldId, CString newVal);
	void put_FieldValue(int FieldId, bool newVal) {	_variables[FieldId]->val->bln(newVal);}
	int get_PartCount() { return _parts.size(); }

	IShape* get_Shape();
	void put_Shape(IShape* shape);

	void SetErrorMessage(CString msg) { _errorMessage = msg; }
	void SetErrorPosition(int position){ _errorPosition = position; }
	void ClearOperations();
	void CacheOperation(COperation& operation);
};