# changelog.sh

# Array of month names
MONTH_NAMES=("January" "February" "March" "April" "May" "June" "July" "August" "September" "October" "November" "December")

# Get the current date
CURRENT_DATE=`date +"%B %_e, %Y"`

# Get the commit number
COMMIT_NUMBER_SHORT=`git show --pretty=format:"%h" --no-patch`                 # "28ada18"
COMMIT_NUMBER_FULL=`git show --pretty=format:"%H" --no-patch`                  # "28ada183890b5af57eee8b342bdffa9ee515b898"

# Get the commit date
COMMIT_DATE=`git show --pretty=format:"%ci" --no-patch | cut -d ' ' -f1`
COMMIT_MONTH=`echo ${COMMIT_DATE} | cut -d '-' -f2`
COMMIT_DAY=`echo ${COMMIT_DATE} | cut -d '-' -f3`
COMMIT_YEAR=`echo ${COMMIT_DATE} | cut -d '-' -f1`

# Get the commit month names
COMMIT_MONTH_NAME=`git show --pretty=format:"%cd" --no-patch | cut -d ' ' -f2`
COMMIT_MONTH_LONG=${MONTH_NAMES[$((10#$COMMIT_MONTH))-1]}

# Build the version number
VERSION_NUMBER="1.${COMMIT_MONTH}.${COMMIT_DAY}.${COMMIT_YEAR:2:2}"            # "1.MM.DD.YY"

# Build the date strings
COMMIT_DATE_SHORT="${COMMIT_MONTH_NAME} ${COMMIT_DAY}, ${COMMIT_YEAR}"         # "Oct 06, 2019"
COMMIT_DATE_FULL="${COMMIT_MONTH_LONG} ${COMMIT_DAY}, ${COMMIT_YEAR}"          # "October 6, 2019"

# Debug output
#echo ${COMMIT_NUMBER_SHORT}
#echo ${COMMIT_NUMBER_FULL}
#echo ${VERSION_NUMBER}
#echo ${COMMIT_DATE_SHORT}
#echo ${COMMIT_DATE_FULL}

# Get the commit number of the previous merge
# I don't think there's any way to get this from git, so we'll get it from the user
PREVIOUS_COMMIT_SHORT="xxxxxxx"
if [ $# -gt 0 ]
then
	PREVIOUS_COMMIT_SHORT=$1
	echo "Using previous commit: ${PREVIOUS_COMMIT_SHORT}"
else
	echo "WARNING! User did not specify previous commit number. Using placeholder text \"${PREVIOUS_COMMIT_SHORT}\""
fi
if [ ${#PREVIOUS_COMMIT_SHORT} -ne 7 ]
then
	echo "WARNING! Previous commit number \"${PREVIOUS_COMMIT_SHORT}\" has length of ${#PREVIOUS_COMMIT_SHORT}, expected length was 7"
fi
echo ""

#################################################
# Output
#################################################

echo "<!>------------------------------------------------------------------------------------------------------------------"
echo "<!> Change the date of the last changelog update (not necessarily the same as the commit date)"
echo "<!>------------------------------------------------------------------------------------------------------------------"
echo ""
echo "Last updated: ${CURRENT_DATE}"
echo ""
echo "<!>------------------------------------------------------------------------------------------------------------------"
echo "<!> Add the new version to the top of the version table (remember to remove \"(current)\" from the previous entry)"
echo "<!>------------------------------------------------------------------------------------------------------------------"
echo ""
echo "| [${VERSION_NUMBER}](#commit_${COMMIT_NUMBER_SHORT}) | ${COMMIT_DATE_SHORT} (current) | [${COMMIT_NUMBER_SHORT}](https://github.com/cthornsb/NEXTSim/tree/${COMMIT_NUMBER_FULL})"
echo ""
echo "<!>------------------------------------------------------------------------------------------------------------------"
echo "<!> Add the new version documentation just below the table"
echo "<!>------------------------------------------------------------------------------------------------------------------"
echo ""
echo "## <a name=\"commit_${COMMIT_NUMBER_SHORT}\"></a>${COMMIT_DATE_FULL}"
echo ""
echo "[${PREVIOUS_COMMIT_SHORT}->${COMMIT_NUMBER_SHORT}](https://github.com/cthornsb/NEXTSim/compare/${PREVIOUS_COMMIT_SHORT}...${COMMIT_NUMBER_SHORT})"
echo ""
echo "### New Commands"
echo ""
echo "### Added"
echo ""
echo "### Changed"
echo ""
echo "### Bug Fixes"
echo ""
echo "### Removed"
