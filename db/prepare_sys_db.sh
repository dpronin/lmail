#!/usr/bin/env bash

set -e

echo "Preparing system and database for lmail ..."

OWNER_GROUP=lmail

DB_OLD_PATH=$1
DB_NEW_PATH=$2

if [ "x${DB_OLD_PATH}" = "x" ]; then
    echo "DB source is not passed"
    exit 1
fi

if [ "x${DB_NEW_PATH}" = "x" ]; then
    echo "DB destination is not passed"
    exit 1
fi

echo "Overwriting '${DB_NEW_PATH}' by '${DB_OLD_PATH}' ..."
cp -ib ${DB_OLD_PATH} ${DB_NEW_PATH}

if [ "x$(cat /etc/group | cut -d: -f1 | sort | uniq | grep ^${OWNER_GROUP}$)" != "x${OWNER_GROUP}" ]; then
    printf "There is no group '${OWNER_GROUP}' in the system. Would you like to create it to support unified access to the database? (y/n)\nAnswer: "
    read ans
    if [ "x${ans}" != "xy" ]; then
        echo "We do not proceed preparing system and database since it is believed you are going to customize it yourself"
        echo "Exiting..."
        exit 0
    fi
    echo "Adding group '${OWNER_GROUP}'..."
    groupadd ${OWNER_GROUP}
fi

DB_DIR=$(dirname ${DB_NEW_PATH})

DB_DIR_G_OWNER="$(stat --format "%G" ${DB_DIR})"
DB_FILE_G_OWNER="$(stat --format "%G" ${DB_NEW_PATH})"

if [ "x${DB_DIR_G_OWNER}" != "x${OWNER_GROUP}" -o "x${DB_FILE_G_OWNER}" != "x${OWNER_GROUP}" ]; then
    printf "Would you like to give access '${OWNER_GROUP}' group to the database? (y/n)\nAnswer: "
    read ans
    if [ "x${ans}" != "xy" ]; then
        echo "We do not proceed preparing system and database since it is believed you are going to customize it yourself"
        echo "Exiting..."
        exit 0
    fi
    echo "Making directory ${DB_DIR} belong to '${OWNER_GROUP}' group ..."
    chown :${OWNER_GROUP} ${DB_DIR}
fi

echo "Making directory ${DB_DIR} be accessible for the owner group '${OWNER_GROUP}'"
chmod g+rwx ${DB_DIR}

if [ "x${DB_FILE_G_OWNER}" != "x${OWNER_GROUP}" ]; then
    echo "Making database ${DB_NEW_PATH} belong to '${OWNER_GROUP}' group"
    chown :${OWNER_GROUP} ${DB_NEW_PATH}
fi

echo "Making database ${DB_NEW_PATH} be read/write accessible for the owner group '${OWNER_GROUP}'"
chmod g+rw ${DB_NEW_PATH}

echo ""
echo "ATTENTION!"
echo "Every user that wants to use '${OWNER_GROUP}' application must be a member of group '${OWNER_GROUP}'"
echo "Adding a user in the group '${OWNER_GROUP}' is done by providing the command 'usermod -aG ${OWNER_GROUP} <your_user>"
echo "Upon adding the user in the group '${OWNER_GROUP}' the user needs to re-login"
echo ""
echo "Exiting..."
