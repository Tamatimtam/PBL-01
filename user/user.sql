-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Nov 07, 2023 at 08:04 AM
-- Server version: 10.4.28-MariaDB
-- PHP Version: 8.2.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `user`
--

-- --------------------------------------------------------

--
-- Table structure for table `lamp_usage_reports`
--

CREATE TABLE `lamp_usage_reports` (
  `id` int(11) NOT NULL,
  `username` varchar(255) NOT NULL,
  `timestamp` datetime NOT NULL,
  `action` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `lamp_usage_reports`
--

INSERT INTO `lamp_usage_reports` (`id`, `username`, `timestamp`, `action`) VALUES
(1, '05-01', '2023-11-07 12:19:09', 'Turn off LED'),
(2, '05-01', '2023-11-07 12:19:17', 'Turn on LED'),
(3, '05-01', '2023-11-07 12:19:17', 'Turn off LED'),
(4, '05-01', '2023-11-07 12:20:54', 'Turn off LED'),
(5, '05-01', '2023-11-07 12:35:35', 'Turn on LED'),
(6, '05-01', '2023-11-07 12:35:36', 'Turn off LED'),
(7, '05-01', '2023-11-07 12:35:37', 'Turn on LED'),
(8, '05-01', '2023-11-07 12:35:38', 'Turn off LED'),
(9, '05-01', '2023-11-07 12:35:54', 'Turn on LED'),
(10, '05-01', '2023-11-07 12:35:56', 'Turn off LED'),
(11, '05-01', '2023-11-07 12:36:08', 'Turn on LED'),
(12, '05-01', '2023-11-07 12:36:32', 'Turn off LED'),
(13, '05-01', '2023-11-07 12:36:33', 'Turn on LED'),
(14, 'caca', '2023-11-07 12:59:21', 'Turn on LED'),
(15, '05-01', '2023-11-07 13:14:21', 'Turn on LED'),
(16, 'caca', '2023-11-07 13:18:45', 'Turn off LED'),
(17, 'caca', '2023-11-07 13:18:50', 'Turn on LED'),
(18, 'caca', '2023-11-07 13:18:55', 'Turn off LED'),
(19, 'caca', '2023-11-07 13:18:57', 'Turn on LED'),
(20, 'caca', '2023-11-07 13:19:04', 'Turn off LED'),
(21, 'caca', '2023-11-07 13:19:09', 'Turn on LED'),
(22, 'caca', '2023-11-07 13:19:15', 'Turn off LED'),
(23, 'caca', '2023-11-07 13:19:22', 'Turn on LED'),
(24, 'caca', '2023-11-07 13:19:36', 'Turn off LED'),
(25, 'caca', '2023-11-07 13:19:40', 'Turn on LED'),
(26, 'caca', '2023-11-07 13:19:45', 'Turn off LED'),
(27, 'caca', '2023-11-07 13:20:02', 'Turn on LED'),
(28, 'caca', '2023-11-07 13:20:12', 'Turn off LED'),
(29, 'caca', '2023-11-07 13:20:17', 'Turn on LED'),
(30, '05-01', '2023-11-07 13:28:25', 'Turn off LED'),
(31, '05-01', '2023-11-07 13:28:27', 'Turn on LED'),
(32, '05-01', '2023-11-07 13:28:28', 'Turn off LED'),
(33, 'caca', '2023-11-07 13:30:19', 'Turn on LED'),
(34, 'caca', '2023-11-07 13:30:46', 'Turn off LED'),
(35, 'caca', '2023-11-07 13:33:38', 'Turn on LED'),
(36, 'caca', '2023-11-07 13:33:43', 'Turn off LED'),
(37, 'caca', '2023-11-07 13:57:30', 'Turn on LED'),
(38, 'caca', '2023-11-07 13:57:32', 'Turn off LED'),
(39, 'caca', '2023-11-07 13:57:43', 'Turn on LED'),
(40, 'caca', '2023-11-07 13:57:47', 'Turn off LED'),
(41, 'caca', '2023-11-07 13:57:48', 'Turn on LED'),
(42, 'caca', '2023-11-07 13:57:49', 'Turn off LED'),
(43, 'caca', '2023-11-07 13:57:50', 'Turn on LED'),
(44, 'caca', '2023-11-07 13:57:51', 'Turn off LED'),
(45, 'caca', '2023-11-07 13:57:52', 'Turn on LED'),
(46, 'caca', '2023-11-07 13:57:53', 'Turn off LED'),
(47, 'caca', '2023-11-07 13:57:53', 'Turn on LED'),
(48, 'caca', '2023-11-07 13:57:54', 'Turn off LED'),
(49, 'caca', '2023-11-07 13:57:54', 'Turn on LED'),
(50, 'caca', '2023-11-07 13:58:07', 'Turn off LED');

-- --------------------------------------------------------

--
-- Table structure for table `login_table`
--

CREATE TABLE `login_table` (
  `ID` int(10) NOT NULL,
  `username` varchar(100) NOT NULL,
  `password` varchar(100) NOT NULL,
  `role` varchar(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `login_table`
--

INSERT INTO `login_table` (`ID`, `username`, `password`, `role`) VALUES
(1, '05-01', '$2b$12$8Fg3qCrmk6b5k01yj9pMluhT2h01zUNFAwrv5aBAabUuMOoZAhquO', 'admin'),
(2, 'ayu', '$2b$12$eTsrXCCneVtCxJb9dbnIcuCQCzRJqskiZJb/j65bGYk5gw1CRL.Wq', 'user'),
(3, 'abian', '$2b$12$cbkpH0o75R9TKyqXc4cHHOJTudsElWAhoPcq4VPCZF52O9F9oUghq', 'user'),
(4, 'azuri', '$2b$12$0GMjRK7oe0Qnot/i6L9SDe6cIQ6rXlfUTYn3DrqbCO3M5ZH9JT2/S', 'admin'),
(5, 'caca', '$2b$12$.JPOzklRUEcRu8R1Fad7e.KatsN52tn0KiS.q.ysaiOoyTeVqLFPq', 'user'),
(6, 'chris', '$2b$12$SWikmWrSt0uVHOZpuM8EEO4/pYjxiDK9MTRaK6xNSjwhp32cpbNdC', 'admin');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--
-- Error reading structure for table user.users: #1932 - Table &#039;user.users&#039; doesn&#039;t exist in engine
-- Error reading data for table user.users: #1064 - You have an error in your SQL syntax; check the manual that corresponds to your MariaDB server version for the right syntax to use near &#039;FROM `user`.`users`&#039; at line 1

--
-- Indexes for dumped tables
--

--
-- Indexes for table `lamp_usage_reports`
--
ALTER TABLE `lamp_usage_reports`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `login_table`
--
ALTER TABLE `login_table`
  ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `lamp_usage_reports`
--
ALTER TABLE `lamp_usage_reports`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=51;

--
-- AUTO_INCREMENT for table `login_table`
--
ALTER TABLE `login_table`
  MODIFY `ID` int(10) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
