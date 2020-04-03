/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   try
   {
      vector<string> Ntokens;
      vector<int> values(3);
      if (CmdExec::lexOptions(option, Ntokens)) {//沒有傳nOpts進去，所以一定會return true。用它來切割option。
         if (Ntokens.size() == 0) {//如果只有cmd(只有MTNew)
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
         }
         if (Ntokens.size() == 1) {//如果有1個token
            if (!myStr2Int(Ntokens[0], values[0])) {//如果這個token不是整數
               if (myStrNCmp("-Array", Ntokens[0], 2) == 0) {//如果這個token代表-Array
                  return CmdExec::errorOption(CMD_OPT_MISSING, Ntokens[0]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
            }
            if (values[0] <= 0) {//如果這個token不是正整數
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
            }
            mtest.newObjs(values[0]);//new MemTestObj values[0]次
            return CMD_EXEC_DONE;
         }
         if (Ntokens.size() == 2) {//如果有2個token
            if (!myStr2Int(Ntokens[0], values[0])) {//如果第1個token不是整數
               if (myStrNCmp("-Array", Ntokens[0], 2) == 0) {//如果第1個token代表-Array
                  if (!myStr2Int(Ntokens[1], values[1])) {//如果第2個token不是整數
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                  }
                  if (values[1] <= 0) {//如果第2個token不是正整數
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                  }
                  return CmdExec::errorOption(CMD_OPT_MISSING, "");
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
            }
            if (values[0] <= 0) {//如果第1個token不是正整數
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
            }
            if (myStrNCmp("-Array", Ntokens[1], 2) == 0) {//如果第2個token代表-Array
               return CmdExec::errorOption(CMD_OPT_MISSING, Ntokens[1]);
            }
            return CmdExec::errorOption(CMD_OPT_EXTRA, Ntokens[1]);
         }
         if (Ntokens.size() == 3) {//如果有3個token
            if (!myStr2Int(Ntokens[0], values[0])) {//如果第1個token不是整數
               if (myStrNCmp("-Array", Ntokens[0], 2) == 0) {//如果第1個token代表-Array
                  if (!myStr2Int(Ntokens[1], values[1])) {//如果第2個token不是整數
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                  }
                  if (values[1] <= 0) {//如果第2個token不是正整數
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                  }
                  if (myStr2Int(Ntokens[2], values[2])) {//如果第3個token是整數
                     if (values[2] <= 0) {//如果第3個token不是正整數
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                     }
                     mtest.newArrs(values[2], values[1]);//new MemTestObj[values[1]] values[2]次
                     return CMD_EXEC_DONE;
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
            }
            if (values[0] <= 0) {//如果第1個token不是正整數
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
            }
            if (myStrNCmp("-Array", Ntokens[1], 2) == 0) {//如果第2個token代表-Array 
               if (myStr2Int(Ntokens[2], values[2])) {//如果第3個token是整數
                  if (values[2] <= 0) {//如果第3個token不是正整數
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                  }
                  mtest.newArrs(values[0], values[2]);//new MemTestObj[values[2]] values[0]次
                  return CMD_EXEC_DONE;
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
            }
            return CmdExec::errorOption(CMD_OPT_EXTRA, Ntokens[1]);
         }
         cerr << "Please check the command!" << endl;
         return CMD_EXEC_ERROR;
      }
   }
   catch(bad_alloc) {
      return CMD_EXEC_ERROR;
   }
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> Ntokens;
   vector<int> values(3);
   int o_size = mtest.getObjListSize(), a_size = mtest.getArrListSize();
   if (CmdExec::lexOptions(option, Ntokens)) {//沒有傳nOpts進去，所以一定會return true。用它來切割option。
      if (Ntokens.size() == 0) {//如果只有cmd(只有MTDelete)
         return CmdExec::errorOption(CMD_OPT_MISSING, "");
      }
      if (Ntokens.size() == 1) {//如果有1個token
         if (myStrNCmp("-Array", Ntokens[0], 2) == 0) {//如果這個token代表-Array
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
         }
         if (myStrNCmp("-Index", Ntokens[0], 2) == 0) {//如果這個token代表-Index
            return CmdExec::errorOption(CMD_OPT_MISSING, Ntokens[0]);
         }
         if (myStrNCmp("-Random", Ntokens[0], 2) == 0) {//如果這個token代表-Random
            return CmdExec::errorOption(CMD_OPT_MISSING, Ntokens[0]);
         }
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
      }
      if (Ntokens.size() == 2) {//如果有2個token
         if (!myStr2Int(Ntokens[0], values[0])) {//如果第1個token不是整數
            if (myStrNCmp("-Array", Ntokens[0], 2) == 0) {//如果第1個token代表-Array
               if (myStrNCmp("-Index", Ntokens[1], 2) == 0) {//如果第2個token代表-Index
                  return CmdExec::errorOption(CMD_OPT_MISSING, Ntokens[1]);
               }
               if (myStrNCmp("-Random", Ntokens[1], 2) == 0) {//如果第2個token代表-Random
                  return CmdExec::errorOption(CMD_OPT_MISSING, Ntokens[1]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
            }
            if (myStrNCmp("-Index", Ntokens[0], 2) == 0) {//如果第1個token代表-Index
               if (myStr2Int(Ntokens[1], values[1])) {//如果第2個token是整數
                  if (values[1] >= 0) {//如果第2個token不是負整數
                     if (values[1] >= o_size) {
                        cerr << "Size of object list (" << o_size << ") is <= " << values[1] << "!!" << endl;
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                     }
                     mtest.deleteObj(values[1]);
                     return CMD_EXEC_DONE;
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
            }
            if (myStrNCmp("-Random", Ntokens[0], 2) == 0) {//如果第1個token代表-Random
               if (myStr2Int(Ntokens[1], values[1])) {//如果第2個token是整數
                  if (values[1] > 0) {//如果第2個token是正整數
                     if (o_size == 0) {
                        cerr << "Size of object list is 0!!" << endl;
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
                     }
                     for (int i = 0; i < values[1]; ++i) {
                        mtest.deleteObj(rnGen(o_size));
                     }
                     return CMD_EXEC_DONE;
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
            }
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
         }
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
      }
      if (Ntokens.size() == 3) {//如果有3個token
         if (!myStr2Int(Ntokens[0], values[0])) {//如果第1個token不是整數
            if (myStrNCmp("-Array", Ntokens[0], 2) == 0) {//如果第1個token代表-Array
               if (myStrNCmp("-Index", Ntokens[1], 2) == 0) {//如果第2個token代表-Index
                  if (myStr2Int(Ntokens[2], values[2])) {//如果第3個token是整數
                     if (values[2] >= 0) {//如果第3個token不是負整數
                        if (values[2] >= a_size) {
                           cerr << "Size of array list (" << a_size << ") is <= " << values[2] << "!!" << endl;
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                        }
                        mtest.deleteArr(values[2]);
                        return CMD_EXEC_DONE;
                     }
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
               }
               if (myStrNCmp("-Random", Ntokens[1], 2) == 0) {//如果第2個token代表-Random
                  if (myStr2Int(Ntokens[2], values[2])) {//如果第3個token是整數
                     if (values[2] > 0) {//如果第3個token是正整數
                        if (a_size == 0) {
                           cerr << "Size of array list is 0!!" << endl;
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                        }
                        for (int i = 0; i < values[2]; ++i) {
                           mtest.deleteArr(rnGen(a_size));
                        }
                        return CMD_EXEC_DONE;
                     }
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
            }
            if (myStrNCmp("-Index", Ntokens[0], 2) == 0) {//如果第1個token代表-Index
               if (myStr2Int(Ntokens[1], values[1])) {//如果第2個token是整數
                  if (values[1] >= 0) {//如果第2個token不是負整數
                     if (myStrNCmp("-Array", Ntokens[2], 2) == 0) {//如果第3個token代表-Array
                        if (values[1] >= a_size) {
                           cerr << "Size of array list (" << a_size << ") is <= " << values[1] << "!!" << endl;
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
                        }
                        mtest.deleteArr(values[1]);
                        return CMD_EXEC_DONE;
                     }
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
            }
            if (myStrNCmp("-Random", Ntokens[0], 2) == 0) {//如果第1個token代表-Random
               if (myStr2Int(Ntokens[1], values[1])) {//如果第2個token是整數
                  if (values[1] > 0) {//如果第2個token是正整數
                     if (myStrNCmp("-Array", Ntokens[2], 2) == 0) {//如果第3個token代表-Array
                        if (a_size == 0) {
                           cerr << "Size of array list is 0!!" << endl;
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
                        }
                        for (int i = 0; i < values[1]; ++i) {
                           mtest.deleteArr(rnGen(a_size));
                        }
                        return CMD_EXEC_DONE;
                     }
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[2]);
                  }
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
               }
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[1]);
            }
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
         }
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, Ntokens[0]);
      }
      cerr << "Please check the command!" << endl;
      return CMD_EXEC_ERROR;
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


