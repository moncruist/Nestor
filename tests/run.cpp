#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include "logger.h"
#include "imap_session_test.h"

using namespace std;


CPPUNIT_TEST_SUITE_REGISTRATION( ImapSessionTest );

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CppUnit::TestResult testresult;

    // register listener for collecting the test-results
    CppUnit::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CppUnit::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CppUnit::TestRunner testrunner;
    testrunner.addTest (CppUnit::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CppUnit::CompilerOutputter compileroutputter(&collectedresults, std::cout);
    compileroutputter.write ();

    // Output XML for Jenkins CPPunit plugin
//    ofstream xmlFileOut("cppTestBasicMathResults.xml");
//    CppUnit::XmlOutputter xmlOut(&collectedresults, xmlFileOut);
//    xmlOut.write();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
