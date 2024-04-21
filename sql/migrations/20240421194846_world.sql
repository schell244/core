DROP PROCEDURE IF EXISTS add_migration;
DELIMITER ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20240421194846');
IF v = 0 THEN
INSERT INTO `migrations` VALUES ('20240421194846');
-- Add your query below.

-- New bell guids
SET @BELL1 := 3998645;
SET @BELL2 := 3998646;
SET @BELL3 := 3998647;
SET @BELL4 := 3998648;

-- Add custom spaws for hourly bells
REPLACE INTO `gameobject` (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecsmin`, `spawntimesecsmax`, `animprogress`, `state`, `spawn_flags`, `visibility_mod`, `patch_min`, `patch_max`) VALUES
(@BELL1,175885,0,1595.68,240.547,-13.8906,6.16331,0,0,0.0599039,-0.998204,25,25,100,1,0,0,0,10), -- undercity
(@BELL2,175885,0,2834.64,-695.866,166.053,3.51012,0,0,0.983072,-0.183222,25,25,100,1,0,0,0,10), -- monastery
(@BELL3,176573,1,9950.75,2317.07,1370.07,4.80421,0,0,0.673908,-0.738815,25,25,100,1,0,0,0,10), -- darnassus
(@BELL4,175885,0,-256.76,1536.6,88.1774,5.83489,0,0,0.222277,-0.974984,25,25,100,1,0,0,0,10); -- shadowfang keep (worgen sound)

-- Link to hourly bell event
REPLACE INTO `game_event_gameobject` (`guid`, `event`) VALUES
(@BELL1,78),
(@BELL2,78),
(@BELL3,78),
(@BELL4,78);

-- End of migration.
END IF;
END??
DELIMITER ;
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
