DROP PROCEDURE IF EXISTS add_migration;
DELIMITER ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20240421095717');
IF v = 0 THEN
INSERT INTO `migrations` VALUES ('20240421095717');
-- Add your query below.

-- Zul'Gurub: Fix LOS issues with High Priestess Jeklik
UPDATE `creature` SET `position_x` =-12290.05, `position_y` =-1382.95, `position_z` = 144.662 WHERE `guid` = 49199;

-- End of migration.
END IF;
END??
DELIMITER ;
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
