-- Run only against MoneyManager_StageA_Test after 001_init_schema.sql.
-- All test rows are rolled back. The password-like value below is a schema
-- placeholder, not a valid Argon2id hash and never a usable credential.
SET NOCOUNT ON;
SET XACT_ABORT OFF;

IF DB_NAME() <> N'MoneyManager_StageA_Test'
    THROW 51100, 'This smoke test is restricted to MoneyManager_StageA_Test.', 1;

IF (SELECT COUNT(*) FROM sys.key_constraints AS k
    JOIN sys.tables AS t ON t.object_id = k.parent_object_id
    WHERE k.type = 'PK' AND t.schema_id = SCHEMA_ID(N'dbo')
      AND t.name IN (N'Users', N'Wallets', N'Categories', N'Transactions')) <> 4
    THROW 51101, 'Expected four primary keys.', 1;

IF (SELECT COUNT(*) FROM sys.key_constraints AS k
    JOIN sys.tables AS t ON t.object_id = k.parent_object_id
    WHERE k.type = 'UQ' AND t.schema_id = SCHEMA_ID(N'dbo')
      AND t.name IN (N'Users', N'Wallets', N'Categories', N'Transactions')) <> 5
    THROW 51115, 'Expected five unique constraints.', 1;

IF (SELECT COUNT(*) FROM sys.foreign_keys AS f
    JOIN sys.tables AS t ON t.object_id = f.parent_object_id
    WHERE t.schema_id = SCHEMA_ID(N'dbo')
      AND t.name IN (N'Users', N'Wallets', N'Categories', N'Transactions')
      AND f.is_disabled = 0 AND f.is_not_trusted = 0) <> 5
    THROW 51116, 'Expected five enabled and trusted foreign keys.', 1;

IF (SELECT COUNT(*) FROM sys.check_constraints AS c
    JOIN sys.tables AS t ON t.object_id = c.parent_object_id
    WHERE t.schema_id = SCHEMA_ID(N'dbo')
      AND t.name IN (N'Users', N'Wallets', N'Categories', N'Transactions')
      AND c.is_disabled = 0 AND c.is_not_trusted = 0) <> 10
    THROW 51117, 'Expected ten enabled and trusted check constraints.', 1;

DECLARE @beforeUsers BIGINT = (SELECT COUNT_BIG(*) FROM dbo.Users);
DECLARE @beforeWallets BIGINT = (SELECT COUNT_BIG(*) FROM dbo.Wallets);
DECLARE @beforeCategories BIGINT = (SELECT COUNT_BIG(*) FROM dbo.Categories);
DECLARE @beforeTransactions BIGINT = (SELECT COUNT_BIG(*) FROM dbo.Transactions);
PRINT 'PASS: PK, UNIQUE, FK and CHECK metadata are present and trusted.';

BEGIN TRY
    BEGIN TRANSACTION;

    DECLARE @suffix NVARCHAR(8) = LEFT(CONVERT(NVARCHAR(36), NEWID()), 8);
    DECLARE @usernameA NVARCHAR(20) = N'stagea_' + @suffix;
    DECLARE @usernameB NVARCHAR(20) = N'stageb_' + @suffix;
    DECLARE @walletName NVARCHAR(200) = N'Ví thử ' + @suffix;
    DECLARE @categoryName NVARCHAR(200) = N'Ăn uống ' + @suffix;
    DECLARE @hash VARCHAR(255) = '$argon2id$stage_a_schema_test_placeholder';
    DECLARE @ids TABLE (id INT NOT NULL);
    DECLARE @userA INT, @userB INT, @walletA INT, @walletB INT;
    DECLARE @incomeA INT, @expenseA INT, @incomeB INT;

    IF UNICODE(SUBSTRING(@walletName, 2, 1)) <> 237
        THROW 51102, 'SQL script Unicode decoding failed.', 1;

    INSERT dbo.Users (username, password_hash, display_name)
        OUTPUT INSERTED.user_id INTO @ids
        VALUES (@usernameA, @hash, N'Người dùng thử A');
    SELECT @userA = id FROM @ids;
    DELETE FROM @ids;

    INSERT dbo.Users (username, password_hash, display_name)
        OUTPUT INSERTED.user_id INTO @ids
        VALUES (@usernameB, @hash, N'Người dùng thử B');
    SELECT @userB = id FROM @ids;
    DELETE FROM @ids;

    INSERT dbo.Wallets (user_id, name, wallet_type, opening_balance)
        OUTPUT INSERTED.wallet_id INTO @ids
        VALUES (@userA, @walletName, 1, 1000);
    SELECT @walletA = id FROM @ids;
    DELETE FROM @ids;

    -- The same wallet name belongs to a different user and is allowed.
    INSERT dbo.Wallets (user_id, name, wallet_type, opening_balance)
        OUTPUT INSERTED.wallet_id INTO @ids
        VALUES (@userB, @walletName, 2, 0);
    SELECT @walletB = id FROM @ids;
    DELETE FROM @ids;

    INSERT dbo.Categories (user_id, name, category_type)
        OUTPUT INSERTED.category_id INTO @ids
        VALUES (@userA, @categoryName, 1);
    SELECT @incomeA = id FROM @ids;
    DELETE FROM @ids;

    -- One account may use the same category name for income and expense.
    INSERT dbo.Categories (user_id, name, category_type)
        OUTPUT INSERTED.category_id INTO @ids
        VALUES (@userA, @categoryName, 2);
    SELECT @expenseA = id FROM @ids;
    DELETE FROM @ids;

    INSERT dbo.Categories (user_id, name, category_type)
        OUTPUT INSERTED.category_id INTO @ids
        VALUES (@userB, @categoryName, 1);
    SELECT @incomeB = id FROM @ids;
    DELETE FROM @ids;

    INSERT dbo.Transactions
        (user_id, wallet_id, category_id, transaction_type, amount, transaction_date, note)
        VALUES
        (@userA, @walletA, @incomeA, 1, 500, DATEFROMPARTS(2026, 1, 2), N'Thu thử'),
        (@userA, @walletA, @expenseA, 2, 200, DATEFROMPARTS(2026, 1, 3), N'Chi thử');

    IF (SELECT UNICODE(SUBSTRING(name, 2, 1)) FROM dbo.Wallets
        WHERE user_id = @userA AND wallet_id = @walletA) <> 237
        OR NOT EXISTS (SELECT 1 FROM dbo.Transactions
            WHERE user_id = @userA AND wallet_id = @walletA
              AND amount = 500 AND note = N'Thu thử')
        THROW 51126, 'Vietnamese text was not stored and retrieved correctly.', 1;

    IF (SELECT w.opening_balance + COALESCE(SUM(CASE WHEN t.transaction_type = 1
            THEN t.amount ELSE -t.amount END), CONVERT(BIGINT, 0))
        FROM dbo.Wallets AS w
        LEFT JOIN dbo.Transactions AS t
            ON t.user_id = w.user_id AND t.wallet_id = w.wallet_id
        WHERE w.user_id = @userA AND w.wallet_id = @walletA
        GROUP BY w.opening_balance) <> 1300
        THROW 51103, 'Derived wallet balance is incorrect.', 1;

    INSERT dbo.Transactions
        (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
        VALUES (@userA, @walletA, @incomeA, 1, CONVERT(BIGINT, 3000000000), DATEFROMPARTS(2026, 1, 5));
    IF (SELECT w.opening_balance + SUM(CASE WHEN t.transaction_type = 1
            THEN t.amount ELSE -t.amount END)
        FROM dbo.Wallets AS w
        JOIN dbo.Transactions AS t
            ON t.user_id = w.user_id AND t.wallet_id = w.wallet_id
        WHERE w.user_id = @userA AND w.wallet_id = @walletA
        GROUP BY w.opening_balance) <> CONVERT(BIGINT, 3000001300)
        THROW 51118, 'BIGINT balance calculation is incorrect.', 1;
    PRINT 'PASS: Unicode, generated IDs, valid relations and BIGINT derived balance.';

    -- UNIQUE: case-insensitive names inside one account.
    BEGIN TRY
        INSERT dbo.Users (username, password_hash, display_name)
            VALUES (UPPER(@usernameA), @hash, N'Duplicate');
        THROW 51104, 'Duplicate username was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() NOT IN (2601, 2627)
            OR CHARINDEX(N'UQ_Users_Username', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Wallets (user_id, name, wallet_type, opening_balance)
            VALUES (@userA, UPPER(@walletName), 1, 0);
        THROW 51105, 'Duplicate wallet name was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() NOT IN (2601, 2627)
            OR CHARINDEX(N'UQ_Wallets_User_Name', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Categories (user_id, name, category_type)
            VALUES (@userA, UPPER(@categoryName), 1);
        THROW 51106, 'Duplicate category name was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() NOT IN (2601, 2627)
            OR CHARINDEX(N'UQ_Categories_User_Type_Name', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;
    PRINT 'PASS: case-insensitive UNIQUE constraints reject duplicates.';

    -- FK: transactions cannot cross user boundaries, even with valid IDs.
    BEGIN TRY
        INSERT dbo.Transactions
            (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
            VALUES (@userB, @walletA, @incomeB, 1, 100, DATEFROMPARTS(2026, 1, 4));
        THROW 51107, 'Cross-user wallet reference was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'FK_Transactions_Wallets', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Transactions
            (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
            VALUES (@userB, @walletB, @incomeA, 1, 100, DATEFROMPARTS(2026, 1, 4));
        THROW 51108, 'Cross-user category reference was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'FK_Transactions_Categories_Type', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Transactions
            (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
            VALUES (@userA, @walletA, @expenseA, 1, 100, DATEFROMPARTS(2026, 1, 4));
        THROW 51109, 'Category/transaction type mismatch was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'FK_Transactions_Categories_Type', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;
    PRINT 'PASS: composite FKs reject cross-user and category/type mismatches.';

    -- CHECK: positive amount, valid enum, supported model date range, Argon2id marker.
    BEGIN TRY
        INSERT dbo.Transactions
            (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
            VALUES (@userA, @walletA, @incomeA, 1, 0, DATEFROMPARTS(2026, 1, 4));
        THROW 51110, 'Zero amount was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Transactions_Amount', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Wallets (user_id, name, wallet_type, opening_balance)
            VALUES (@userA, N'Invalid type', 9, 0);
        THROW 51111, 'Invalid wallet type was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Wallets_Type', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Transactions
            (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
            VALUES (@userA, @walletA, @incomeA, 1, 100, DATEFROMPARTS(2101, 1, 1));
        THROW 51112, 'Date outside the model range was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Transactions_Date', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Users (username, password_hash, display_name)
            VALUES (N'bad_' + @suffix, 'plaintext', N'Invalid');
        THROW 51113, 'Plaintext-shaped password was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Users_PasswordHash', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Users (username, password_hash, display_name)
            VALUES (N'bad-name', @hash, N'Invalid');
        THROW 51119, 'Invalid username characters were accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Users_Username', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Users (username, password_hash, display_name)
            VALUES (N'blank_' + @suffix, @hash, N'');
        THROW 51120, 'Empty display name was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Users_DisplayName', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Wallets (user_id, name, wallet_type, opening_balance)
            VALUES (@userA, N'', 1, 0);
        THROW 51121, 'Empty wallet name was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Wallets_Name', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Categories (user_id, name, category_type)
            VALUES (@userA, N'', 1);
        THROW 51122, 'Empty category name was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Categories_Name', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Categories (user_id, name, category_type)
            VALUES (@userA, N'Invalid type', 9);
        THROW 51123, 'Invalid category type was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Categories_Type', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;

    BEGIN TRY
        INSERT dbo.Transactions
            (user_id, wallet_id, category_id, transaction_type, amount, transaction_date)
            VALUES (@userA, @walletA, @incomeA, 9, 100, DATEFROMPARTS(2026, 1, 4));
        THROW 51124, 'Invalid transaction type was accepted.', 1;
    END TRY
    BEGIN CATCH
        IF ERROR_NUMBER() <> 547
            OR CHARINDEX(N'CK_Transactions_Type', ERROR_MESSAGE()) = 0 THROW;
    END CATCH;
    PRINT 'PASS: all ten CHECK constraints reject their invalid examples.';

    ROLLBACK TRANSACTION;

    DECLARE @rollbackUsername NVARCHAR(20) = N'rollback_' + @suffix;
    BEGIN TRY
        BEGIN TRANSACTION;
        INSERT dbo.Users (username, password_hash, display_name)
            VALUES (@rollbackUsername, @hash, N'Rollback test');
        INSERT dbo.Wallets (user_id, name, wallet_type, opening_balance)
            VALUES (-1, N'Invalid owner', 1, 0);
        THROW 51125, 'A failed multi-step write was accepted.', 1;
    END TRY
    BEGIN CATCH
        DECLARE @writeError INT = ERROR_NUMBER();
        DECLARE @writeMessage NVARCHAR(4000) = ERROR_MESSAGE();
        IF @@TRANCOUNT > 0 ROLLBACK TRANSACTION;
        IF @writeError <> 547
            OR CHARINDEX(N'FK_Wallets_Users', @writeMessage) = 0 THROW;
    END CATCH;

    IF (SELECT COUNT_BIG(*) FROM dbo.Users) <> @beforeUsers
        OR (SELECT COUNT_BIG(*) FROM dbo.Wallets) <> @beforeWallets
        OR (SELECT COUNT_BIG(*) FROM dbo.Categories) <> @beforeCategories
        OR (SELECT COUNT_BIG(*) FROM dbo.Transactions) <> @beforeTransactions
        THROW 51114, 'Table counts changed after rollback.', 1;

    PRINT 'PASS: test rollback and failed multi-step write preserved all four table counts.';
END TRY
BEGIN CATCH
    IF @@TRANCOUNT > 0 ROLLBACK TRANSACTION;
    THROW;
END CATCH;
