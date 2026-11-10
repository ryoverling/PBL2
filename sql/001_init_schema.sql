-- Stage A only: run against the new, empty MoneyManager_StageA_Test database.
-- All amounts are integral VND. Current balance is derived from opening_balance
-- plus signed transactions; it is not stored a second time.
SET NOCOUNT ON;
SET XACT_ABORT ON;

IF DB_NAME() <> N'MoneyManager_StageA_Test'
    THROW 51010, 'This Stage A schema script is restricted to MoneyManager_StageA_Test.', 1;

IF EXISTS (SELECT 1 FROM sys.tables WHERE is_ms_shipped = 0)
    THROW 51011, 'The database already has user tables; nothing was changed.', 1;

BEGIN TRY
    BEGIN TRANSACTION;

    CREATE TABLE dbo.Users (
        user_id INT IDENTITY(1,1) NOT NULL,
        username NVARCHAR(20) COLLATE Latin1_General_100_CI_AS_SC NOT NULL,
        password_hash VARCHAR(255) COLLATE Latin1_General_100_BIN2 NOT NULL,
        display_name NVARCHAR(200) NOT NULL,
        CONSTRAINT PK_Users PRIMARY KEY (user_id),
        CONSTRAINT UQ_Users_Username UNIQUE (username),
        CONSTRAINT CK_Users_Username CHECK (
            LEN(username) BETWEEN 1 AND 20
            AND username COLLATE Latin1_General_100_BIN2
                NOT LIKE N'%[^A-Za-z0-9_]%' COLLATE Latin1_General_100_BIN2
        ),
        CONSTRAINT CK_Users_PasswordHash CHECK (
            LEN(password_hash) BETWEEN 20 AND 255
            AND LEFT(password_hash, 10) = '$argon2id$'
        ),
        CONSTRAINT CK_Users_DisplayName CHECK (LEN(display_name) > 0)
    );

    CREATE TABLE dbo.Wallets (
        wallet_id INT IDENTITY(1,1) NOT NULL,
        user_id INT NOT NULL,
        name NVARCHAR(200) COLLATE Latin1_General_100_CI_AS_SC NOT NULL,
        wallet_type TINYINT NOT NULL,
        opening_balance BIGINT NOT NULL,
        CONSTRAINT PK_Wallets PRIMARY KEY (wallet_id),
        CONSTRAINT UQ_Wallets_User_Wallet UNIQUE (user_id, wallet_id),
        CONSTRAINT UQ_Wallets_User_Name UNIQUE (user_id, name),
        CONSTRAINT FK_Wallets_Users FOREIGN KEY (user_id) REFERENCES dbo.Users(user_id),
        CONSTRAINT CK_Wallets_Name CHECK (LEN(name) > 0),
        CONSTRAINT CK_Wallets_Type CHECK (wallet_type IN (1, 2, 3, 4))
    );

    CREATE TABLE dbo.Categories (
        category_id INT IDENTITY(1,1) NOT NULL,
        user_id INT NOT NULL,
        name NVARCHAR(200) COLLATE Latin1_General_100_CI_AS_SC NOT NULL,
        category_type TINYINT NOT NULL,
        CONSTRAINT PK_Categories PRIMARY KEY (category_id),
        CONSTRAINT UQ_Categories_User_Category_Type UNIQUE (user_id, category_id, category_type),
        CONSTRAINT UQ_Categories_User_Type_Name UNIQUE (user_id, category_type, name),
        CONSTRAINT FK_Categories_Users FOREIGN KEY (user_id) REFERENCES dbo.Users(user_id),
        CONSTRAINT CK_Categories_Name CHECK (LEN(name) > 0),
        CONSTRAINT CK_Categories_Type CHECK (category_type IN (1, 2))
    );

    CREATE TABLE dbo.Transactions (
        transaction_id INT IDENTITY(1,1) NOT NULL,
        user_id INT NOT NULL,
        wallet_id INT NOT NULL,
        category_id INT NOT NULL,
        transaction_type TINYINT NOT NULL,
        amount BIGINT NOT NULL,
        transaction_date DATE NOT NULL,
        note NVARCHAR(MAX) NOT NULL CONSTRAINT DF_Transactions_Note DEFAULT N'',
        CONSTRAINT PK_Transactions PRIMARY KEY (transaction_id),
        CONSTRAINT FK_Transactions_Users FOREIGN KEY (user_id) REFERENCES dbo.Users(user_id),
        CONSTRAINT FK_Transactions_Wallets FOREIGN KEY (user_id, wallet_id)
            REFERENCES dbo.Wallets(user_id, wallet_id),
        CONSTRAINT FK_Transactions_Categories_Type FOREIGN KEY (user_id, category_id, transaction_type)
            REFERENCES dbo.Categories(user_id, category_id, category_type),
        CONSTRAINT CK_Transactions_Type CHECK (transaction_type IN (1, 2)),
        CONSTRAINT CK_Transactions_Amount CHECK (amount > 0),
        CONSTRAINT CK_Transactions_Date CHECK (
            transaction_date >= DATEFROMPARTS(1900, 1, 1)
            AND transaction_date <= DATEFROMPARTS(2100, 12, 31)
        )
    );

    CREATE INDEX IX_Transactions_User_Date
        ON dbo.Transactions(user_id, transaction_date DESC, transaction_id DESC)
        INCLUDE (wallet_id, category_id, transaction_type, amount);

    CREATE INDEX IX_Transactions_User_Wallet_Date
        ON dbo.Transactions(user_id, wallet_id, transaction_date)
        INCLUDE (transaction_type, amount);

    CREATE INDEX IX_Transactions_User_Category_Date
        ON dbo.Transactions(user_id, category_id, transaction_date)
        INCLUDE (transaction_type, amount);

    COMMIT TRANSACTION;
END TRY
BEGIN CATCH
    IF @@TRANCOUNT > 0 ROLLBACK TRANSACTION;
    THROW;
END CATCH;
