/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: baderwae <baderwae@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 19:38:49 by baderwae          #+#    #+#             */
/*   Updated: 2026/01/06 19:38:49 by baderwae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>


static int	is_sep(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static int	count_words(char *str)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*str)
	{
		if (!is_sep(*str) && !in_word)
		{
			count++;
			in_word = 1;
		}
		else if (is_sep(*str))
			in_word = 0;
		str++;
	}
	return (count);
}

static char	*word_dup(char *start, char *end)
{
	char	*word;
	int		i;

	word = (char *)malloc(sizeof(char) * (end - start + 1));
	if (!word)
		return (NULL);
	i = 0;
	while (start < end)
		word[i++] = *start++;
	word[i] = '\0';
	return (word);
}

char	**ft_split(char *str)
{
	char	**result;
	char	*word_start;
	int		i;

	if (!str)
        return NULL;
	result = (char **)malloc(sizeof(char *) * (count_words(str) + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (*str)
	{
		while (*str && is_sep(*str))
			str++;
		if (*str)
		{
			word_start = str;
			while (*str && !is_sep(*str))
				str++;
			result[i++] = word_dup(word_start, str);
		}
	}
	result[i] = NULL;
	return (result);
}
