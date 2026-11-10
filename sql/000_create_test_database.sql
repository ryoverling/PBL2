-- Run against master with Windows Authentication.
-- This script creates only the dedicated, empty Stage A database.
SET NOCOUNT ON;

IF DB_NAME() <> N'master'
    THROW 51000, 'Run this script against master.', 1;

IF DB_ID(N'MoneyManager_StageA_Test') IS NOT NULL
    THROW 51001, 'The Stage A test database already exists; nothing was changed.', 1;

EXEC(N'CREATE DATABASE [MoneyManager_StageA_Test] COLLATE Latin1_General_100_CI_AS_SC');
