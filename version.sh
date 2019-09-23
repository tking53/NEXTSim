# Get the commit number
COMMIT_NUMBER=`git show --pretty=format:"%h" --no-patch`

# Get the commit date
COMMIT_DATE=`git show --pretty=format:"%ci" --no-patch | cut -d ' ' -f1`

# Get the current branch
COMMIT_BRANCH=`git branch | grep \* | cut -d ' ' -f2`

echo $COMMIT_NUMBER"-"$COMMIT_BRANCH" ("$COMMIT_DATE")"
