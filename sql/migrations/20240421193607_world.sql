DROP PROCEDURE IF EXISTS add_migration;
DELIMITER ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20240421193607');
IF v = 0 THEN
INSERT INTO `migrations` VALUES ('20240421193607');
-- Add your query below.

DELETE FROM `script_texts` WHERE `entry`= -1000600; 
DELETE FROM `script_texts` WHERE `entry`= -1000601;

-- End of migration.
END IF;
END??
DELIMITER ;
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
