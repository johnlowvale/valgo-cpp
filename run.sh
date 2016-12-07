
#notes:
#please save this file with linux line ending or error will happen on blank
#lines

tput setaf 4
echo "Setting environment variables..."
tput sgr0
source ./env.sh
echo "Set."
echo ""

echo "Removing old executable..."
echo "Before removal:"
ls build
rm build/*
echo "After removal:"
ls build
echo "Removed."
echo ""
cd src

echo "Compiling miscs files..."
cd miscs
#-------------------------------------------------------------------------------
md5sum utils.cpp >temp.md5
if cmp -s temp.md5 utils.md5
then
  echo "utils.cpp (unchanged)"
else
  echo "utils.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o utils.o utils.cpp
  then
    md5sum utils.cpp >utils.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi

md5sum db.cpp >temp.md5
if cmp -s temp.md5 db.md5
then
  echo "db.cpp (unchanged)"
else
  echo "db.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o db.o db.cpp
  then
    md5sum db.cpp >db.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
#-------------------------------------------------------------------------------
cd ..
echo "All miscs files compiled."
echo ""

echo "Compiling tasks files..."
cd tasks
#-------------------------------------------------------------------------------
md5sum crawler.cpp >temp.md5
if cmp -s temp.md5 crawler.md5
then
  echo "crawler.cpp (unchanged)"
else
  echo "crawler.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o crawler.o crawler.cpp
  then
    md5sum crawler.cpp >crawler.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
#-------------------------------------------------------------------------------
cd ..
echo "All tasks files compiled."
echo ""

echo "Compiling entities files..."
cd entities
#-------------------------------------------------------------------------------
md5sum moment.cpp >temp.md5
if cmp -s temp.md5 moment.md5
then
  echo "moment.cpp (unchanged)"
else
  echo "moment.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o moment.o moment.cpp
  then
    md5sum moment.cpp >moment.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi

md5sum webloc.cpp >temp.md5
if cmp -s temp.md5 webloc.md5
then
  echo "webloc.cpp (unchanged)"
else
  echo "webloc.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o webloc.o webloc.cpp
  then
    md5sum webloc.cpp >webloc.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi

md5sum content.cpp >temp.md5
if cmp -s temp.md5 content.md5
then
  echo "content.cpp (unchanged)"
else
  echo "content.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o content.o content.cpp
  then
    md5sum content.cpp >content.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi

md5sum node.cpp >temp.md5
if cmp -s temp.md5 node.md5
then
  echo "node.cpp (unchanged)"
else
  echo "node.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o node.o node.cpp
  then
    md5sum node.cpp >node.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi

md5sum relation.cpp >temp.md5
if cmp -s temp.md5 relation.md5
then
  echo "relation.cpp (unchanged)"
else
  echo "relation.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o relation.o relation.cpp
  then
    md5sum relation.cpp >relation.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi

md5sum graph.cpp >temp.md5
if cmp -s temp.md5 graph.md5
then
  echo "graph.cpp (unchanged)"
else
  echo "graph.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o graph.o graph.cpp
  then
    md5sum graph.cpp >graph.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
#-------------------------------------------------------------------------------
cd ..
echo "All entities files compiled."
echo ""

echo "Compiling algos files..."
#-------------------------------------------------------------------------------
cd algos/searching
md5sum searcher.cpp >temp.md5
if cmp -s temp.md5 searcher.md5
then
  echo "searcher.cpp (unchanged)"
else
  echo "searcher.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o searcher.o searcher.cpp
  then
    md5sum searcher.cpp >searcher.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
cd ../..
#-------------------------------------------------------------------------------
cd algos/tipping
md5sum tipper.cpp >temp.md5
if cmp -s temp.md5 tipper.md5
then
  echo "tipper.cpp (unchanged)"
else
  echo "tipper.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o tipper.o tipper.cpp
  then
    md5sum tipper.cpp >tipper.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
cd ../..
#-------------------------------------------------------------------------------
cd algos/learning
md5sum neunet.cpp >temp.md5
if cmp -s temp.md5 neunet.md5
then
  echo "neunet.cpp (unchanged)"
else
  echo "neunet.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o neunet.o neunet.cpp
  then
    md5sum neunet.cpp >neunet.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
cd ../..
#-------------------------------------------------------------------------------
echo "All algos files compiled."
echo ""

echo "Compiling main code file..."
#-------------------------------------------------------------------------------
md5sum server.cpp >temp.md5
if cmp -s temp.md5 server.md5
then
  echo "server.cpp (unchanged)"
else
  echo "server.cpp is being compiled..."
  if g++-6 -std=c++14 -Wall -Wfatal-errors -c -o server.o server.cpp
  then
    md5sum server.cpp >server.md5
    tput setaf 2
    echo "(compiled)"
    tput sgr0
  else
    tput setaf 1
    echo "(failed)"
    tput sgr0
    exit
  fi
fi
#-------------------------------------------------------------------------------
echo "Main code file compiled."
echo ""

echo "Copying object files to build dir..."
cp server.o ../build
cp miscs/*.o ../build
cp tasks/*.o ../build
cp entities/*.o ../build
cp algos/searching/*.o ../build
cp algos/tipping/*.o ../build
cp algos/learning/*.o ../build
echo "Copied."
echo ""

echo "Linking object files to executable..."
cd ../build
g++-6 -std=c++14 -Wall -Wfatal-errors -pthread -o server \
server.o searcher.o tipper.o neunet.o crawler.o utils.o db.o moment.o \
webloc.o content.o relation.o node.o graph.o \
-lboost_system -lboost_filesystem -lboost_regex -lbsoncxx -lmongocxx \
-luriparser -lhtmlcxx
echo "Linked."
echo ""

echo "Setting execution permission..."
chmod 777 server
echo "Permission set."
echo ""

tput setaf 4
echo "Running main executable..."
tput sgr0
./server
tput setaf 4
echo "Executable finished."
tput sgr0

#end of file
